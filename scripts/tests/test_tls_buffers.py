"""Reproduce the reported TLS NULL counter using the installed IDF source.
The host harness executes the actual pointer reset and counter check; transport,
handshake and the task scheduler are not an ESP32 hardware simulation.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest


def function(source, signature):
    start = source.index(signature)
    brace = source.index('{', start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


class TLSBufferTests(unittest.TestCase):
    def test_null_counter_reproduction_and_serialized_stress(self):
        root = Path(__file__).resolve().parents[2]
        idf = Path(os.environ.get('IDF_PATH', Path.home()/'.espressif/v5.3.5/esp-idf'))
        base = idf/'components/mbedtls'
        dynamic = (base/'port/dynamic/esp_mbedtls_dynamic_impl.c').read_text()
        ssl_msg = (base/'mbedtls/library/ssl_msg.c').read_text()
        reset = function(dynamic, 'static void init_rx_buffer(')
        check = function(ssl_msg, 'static int ssl_check_ctr_renegotiate(')
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#define MBEDTLS_PRIVATE(x) x
#define MBEDTLS_SSL_HEADER_LEN 13
#define MBEDTLS_SSL_SEQUENCE_NUMBER_LEN 8
#define MBEDTLS_SSL_RENEGOTIATION_PENDING 1
#define MBEDTLS_SSL_RENEGOTIATION_DISABLED 1
#define MBEDTLS_SSL_DEBUG_MSG(...) ((void)0)
#define ESP_LOGV(...) ((void)0)
typedef struct {int disable_renegotiation;unsigned char renego_period[8];} config;
typedef struct {unsigned char *in_buf,*in_ctr,*in_hdr,*in_len,*in_iv,*in_msg;unsigned char cur_out_ctr[8];int renego_status,in_msgtype;size_t in_msglen,in_left;config *conf;} mbedtls_ssl_context;
static size_t mbedtls_ssl_ep_len(mbedtls_ssl_context *s) {(void)s;return 0;}
static int mbedtls_ssl_is_handshake_over(mbedtls_ssl_context *s) {(void)s;return 1;}
static int mbedtls_ssl_renegotiate(mbedtls_ssl_context *s) {(void)s;return -1;}
''' + reset + '\n' + check + r'''
static mbedtls_ssl_context ssl;
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static unsigned char records[64];
static void *reader(void *unused) {
 (void)unused;
 for(unsigned i=0;i<100000;i++) {
  pthread_mutex_lock(&lock);
  // Fixed buffers: the record/counter storage lives for the whole connection.
  assert(ssl.in_ctr==records);ssl.in_ctr[7]=(unsigned char)i;
  pthread_mutex_unlock(&lock);
 }
 return NULL;
}
static void *writer(void *unused) {
 (void)unused;
 for(unsigned i=0;i<100000;i++) {
  pthread_mutex_lock(&lock);assert(ssl_check_ctr_renegotiate(&ssl)==0);
  pthread_mutex_unlock(&lock);
 }
 return NULL;
}
int main(int argc,char **argv) {
 config cfg={0};memset(cfg.renego_period,0xff,8);ssl.conf=&cfg;
 ssl.in_msg=(unsigned char *)13;init_rx_buffer(&ssl,records);
 if(argc>1&&!strcmp(argv[1],"legacy-null-window")) {
  // Exact state between the dynamic RX free and replacement allocation.
  init_rx_buffer(&ssl,NULL);assert(ssl.in_ctr==NULL);
  return ssl_check_ctr_renegotiate(&ssl);
 }
 pthread_t rx,tx;assert(!pthread_create(&rx,NULL,reader,NULL));assert(!pthread_create(&tx,NULL,writer,NULL));
 assert(!pthread_join(rx,NULL));assert(!pthread_join(tx,NULL));
 assert(ssl.in_ctr==records);puts("PASS 100000 RX + 100000 TX counter checks with stable records/shared lock");
}
'''
        with tempfile.TemporaryDirectory() as tmp:
            p = Path(tmp); (p/'test.c').write_text(code)
            subprocess.run(['cc','-std=c11','-O1','-g','-pthread','-fsanitize=address,undefined',
                            '-Wno-pointer-to-int-cast','-Wno-int-to-pointer-cast',str(p/'test.c'),'-o',str(p/'test')],check=True)
            env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=0')
            old = subprocess.run([str(p/'test'),'legacy-null-window'],capture_output=True,text=True,env=env,timeout=15)
            self.assertNotEqual(old.returncode,0,'The old NULL-counter window must reproduce the crash')
            self.assertIn('ssl_check_ctr_renegotiate',old.stderr)
            self.assertIn('AddressSanitizer',old.stderr)
            new = subprocess.run([str(p/'test')],capture_output=True,text=True,env=env,timeout=30)
            self.assertEqual(new.returncode,0,new.stdout+new.stderr)
            print('PASS old dynamic RX NULL window reproduced in installed TLS source')
            print(new.stdout,end='')
        # Bind the tested policy to the project rather than merely testing a model.
        for name in ['sdkconfig','sdkconfig.defaults']:
            config_text=(root/name).read_text()
            self.assertIn('# CONFIG_MBEDTLS_DYNAMIC_BUFFER is not set',config_text)
            self.assertIn('# CONFIG_ESP_WS_CLIENT_SEPARATE_TX_LOCK is not set',config_text)
            self.assertIn('CONFIG_MBEDTLS_EXTERNAL_MEM_ALLOC=y',config_text)
        ws=(root/'managed_components/espressif__esp_websocket_client/esp_websocket_client.c').read_text()
        send=function(ws,'static int esp_websocket_client_send_with_exact_opcode(')
        self.assertIn('xSemaphoreTakeRecursive(client->lock, timeout)',send)
        self.assertIn('xSemaphoreTakeRecursive(client->lock, lock_timeout);\n                if (esp_websocket_client_recv(client)',ws)
        self.assertIn('esp_transport_poll_read(client->transport, 1000)',ws)


if __name__=='__main__': unittest.main()
