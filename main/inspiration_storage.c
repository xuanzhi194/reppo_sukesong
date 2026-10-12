#define _POSIX_C_SOURCE 200809L
#include "inspiration_storage.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#ifdef ESP_PLATFORM
#include "esp_attr.h"
#define IDEA_RAM EXT_RAM_BSS_ATTR
#else
#define IDEA_RAM
#endif

/* Fixed slots keep deletion/reuse bounded without a second full-file copy.
 * Commit the CRC-protected header only after the payload reaches storage. */
#define MAGIC 0x31414549u /* IEA1, versioned file format */
_Static_assert(sizeof(inspiration_record_t) + 12 == INSPIRATION_RECORD_BYTES, "record layout");
static uint32_t s_offsets[INSPIRATION_LIMIT] IDEA_RAM;
static uint32_t s_ids[INSPIRATION_LIMIT] IDEA_RAM;
static inspiration_record_t s_scratch IDEA_RAM;
static char s_path[192];
static unsigned s_count, s_slots;
static uint32_t s_next_id;
static bool s_ready;

void inspiration_text_copy(char *dst, size_t cap, const char *src)
{
    size_t n = 0;
    if (!dst || !cap) return;
    if (!src) src = "";
    while (*src) {
        unsigned char c = (unsigned char)*src;
        size_t width = c < 0x80 ? 1 : (c >= 0xc2 && c <= 0xdf ? 2 :
                       (c >= 0xe0 && c <= 0xef ? 3 : (c >= 0xf0 && c <= 0xf4 ? 4 : 0)));
        if (!width) { ++src; continue; }
        bool valid = true;
        for (size_t i = 1; i < width; ++i) {
            if (!src[i] || ((unsigned char)src[i] & 0xc0) != 0x80) { valid = false; break; }
        }
        if (valid && width >= 3) {
            unsigned char b = (unsigned char)src[1];
            if ((c == 0xe0 && b < 0xa0) || (c == 0xed && b >= 0xa0) ||
                (c == 0xf0 && b < 0x90) || (c == 0xf4 && b >= 0x90)) valid = false;
        }
        if (!valid) { ++src; continue; }
        if (n + width >= cap) break;
        memcpy(dst + n, src, width);
        n += width;
        src += width;
    }
    dst[n] = '\0';
}

static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static void put32(uint8_t *p, uint32_t v)
{
    for (unsigned i = 0; i < 4; ++i) p[i] = (uint8_t)(v >> (i * 8));
}
static uint32_t checksum(const void *data, size_t len, uint32_t id)
{
    const uint8_t *p = data;
    uint32_t crc = ~id;
    while (len--) {
        crc ^= *p++;
        for (unsigned i = 0; i < 8; ++i) crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
    }
    return ~crc;
}
static bool valid_record(const inspiration_record_t *r)
{
    return memchr(r->time, 0, sizeof(r->time)) && memchr(r->title, 0, sizeof(r->title)) &&
           memchr(r->original, 0, sizeof(r->original)) && memchr(r->text, 0, sizeof(r->text)) &&
           r->title[0] && r->text[0];
}
static bool finish(FILE *f)
{
    bool ok = fflush(f) == 0;
#ifndef ESP_PLATFORM
    if (ok && fsync(fileno(f)) != 0) ok = false;
#endif
    /* ESP-IDF SPIFFS has no VFS fsync; SPIFFS_close flushes its write cache. */
    if (fclose(f) != 0) ok = false;
    return ok;
}

inspiration_status_t inspiration_storage_init(const char *path)
{
    s_ready = false;
    s_count = s_slots = 0;
    s_next_id = 1;
    if (!path || strlen(path) >= sizeof(s_path)) return INSPIRATION_INVALID;
    if (path != s_path) strcpy(s_path, path);
    FILE *f = fopen(path, "r+b");
    if (!f && errno == ENOENT) {
        f = fopen(path, "w+b");
        if (!f) return INSPIRATION_IO;
    }
    if (!f) return INSPIRATION_IO;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return INSPIRATION_IO; }
    long bytes = ftell(f);
    if (bytes < 0 || (unsigned long)bytes > INSPIRATION_LIMIT * INSPIRATION_RECORD_BYTES) {
        fclose(f); return INSPIRATION_INVALID;
    }
    s_slots = (unsigned long)bytes / INSPIRATION_RECORD_BYTES;
    /* Power loss during an append leaves a partial final slot. Keep all earlier records. */
    if ((unsigned long)bytes % INSPIRATION_RECORD_BYTES &&
        ftruncate(fileno(f), (off_t)s_slots * INSPIRATION_RECORD_BYTES) != 0) {
        fclose(f); return INSPIRATION_IO;
    }
    rewind(f);
    for (unsigned slot = 0; slot < s_slots; ++slot) {
        uint8_t h[12];
        if (fread(h, 1, sizeof(h), f) != sizeof(h) ||
            fread(&s_scratch, 1, sizeof(s_scratch), f) != sizeof(s_scratch)) {
            fclose(f); return INSPIRATION_IO;
        }
        uint32_t id = get32(h + 4);
        if (get32(h) != MAGIC || !id || !valid_record(&s_scratch) ||
            checksum(&s_scratch, sizeof(s_scratch), id) != get32(h + 8)) continue;
        unsigned at = s_count;
        while (at && s_ids[at - 1] > id) {
            s_ids[at] = s_ids[at - 1]; s_offsets[at] = s_offsets[at - 1]; --at;
        }
        s_ids[at] = id; s_offsets[at] = slot * INSPIRATION_RECORD_BYTES; ++s_count;
        if (id >= s_next_id) s_next_id = id == UINT32_MAX ? 0 : id + 1;
    }
    bool ok = !ferror(f);
    if (fclose(f) != 0) ok = false;
    s_ready = ok;
    return ok ? INSPIRATION_OK : INSPIRATION_IO;
}

unsigned inspiration_storage_count(void) { return s_count; }

inspiration_status_t inspiration_storage_read(unsigned newest, inspiration_record_t *out)
{
    if (!s_ready) return INSPIRATION_IO;
    if (!out || newest >= s_count) return INSPIRATION_INVALID;
    FILE *f = fopen(s_path, "rb");
    if (!f) return INSPIRATION_IO;
    uint8_t h[12];
    unsigned at = s_count - 1 - newest;
    bool ok = fseek(f, s_offsets[at], SEEK_SET) == 0 && fread(h, 1, 12, f) == 12 &&
              fread(out, 1, sizeof(*out), f) == sizeof(*out);
    if (fclose(f) != 0) ok = false;
    if (!ok) return INSPIRATION_IO;
    if (get32(h) != MAGIC || get32(h + 4) != s_ids[at] || !valid_record(out) ||
        checksum(out, sizeof(*out), s_ids[at]) != get32(h + 8)) return INSPIRATION_INVALID;
    return INSPIRATION_OK;
}

inspiration_status_t inspiration_storage_add(const inspiration_record_t *r)
{
    if (!s_ready) return INSPIRATION_IO;
    if (!r || !valid_record(r)) return INSPIRATION_INVALID;
    if (s_count >= INSPIRATION_LIMIT || !s_next_id) return INSPIRATION_FULL;
    unsigned slot;
    for (slot = 0; slot < s_slots; ++slot) {
        bool used = false;
        for (unsigned i = 0; i < s_count; ++i) if (s_offsets[i] == slot * INSPIRATION_RECORD_BYTES) used = true;
        if (!used) break;
    }
    FILE *f = fopen(s_path, "r+b");
    if (!f && errno == ENOENT) f = fopen(s_path, "w+b");
    if (!f) return INSPIRATION_IO;
    uint8_t h[12] = {0};
    uint32_t offset = slot * INSPIRATION_RECORD_BYTES;
    bool ok = fseek(f, offset, SEEK_SET) == 0 && fwrite(h, 1, 12, f) == 12 &&
              fwrite(r, 1, sizeof(*r), f) == sizeof(*r);
    if (!finish(f)) ok = false;
    f = NULL;
    if (ok) {
        f = fopen(s_path, "r+b");
        if (!f) ok = false;
    }
    if (ok) {
        put32(h, MAGIC); put32(h + 4, s_next_id); put32(h + 8, checksum(r, sizeof(*r), s_next_id));
        ok = fseek(f, offset, SEEK_SET) == 0 && fwrite(h, 1, 12, f) == 12;
    }
    if (f && !finish(f)) ok = false;
    if (!ok) { (void)inspiration_storage_init(s_path); return INSPIRATION_IO; }
    if (slot == s_slots) ++s_slots;
    s_offsets[s_count] = offset; s_ids[s_count++] = s_next_id++;
    return INSPIRATION_OK;
}

inspiration_status_t inspiration_storage_delete(unsigned newest)
{
    if (!s_ready) return INSPIRATION_IO;
    if (newest >= s_count) return INSPIRATION_INVALID;
    unsigned at = s_count - 1 - newest;
    FILE *f = fopen(s_path, "r+b");
    if (!f) return INSPIRATION_IO;
    uint8_t dead[4] = {0};
    bool ok = fseek(f, s_offsets[at], SEEK_SET) == 0 && fwrite(dead, 1, 4, f) == 4;
    if (f && !finish(f)) ok = false;
    if (!ok) { (void)inspiration_storage_init(s_path); return INSPIRATION_IO; }
    memmove(s_offsets + at, s_offsets + at + 1, (s_count - at - 1) * sizeof(*s_offsets));
    memmove(s_ids + at, s_ids + at + 1, (s_count - at - 1) * sizeof(*s_ids));
    --s_count;
    return INSPIRATION_OK;
}
