#define _POSIX_C_SOURCE 200809L
#include "inspiration_storage.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    assert(argc == 2);
    const char *path = argv[1];
    inspiration_record_t r = {0}, out;
    inspiration_text_copy(r.time, sizeof(r.time), "2026-10-12 14:35");
    inspiration_text_copy(r.title, sizeof(r.title), "让灵感有一个安静的家");
    inspiration_text_copy(r.original, sizeof(r.original), "突然想到，那个页面可以有点像一本小小的笔记。\n也保留原话。🙂");
    inspiration_text_copy(r.text, sizeof(r.text), "把页面做成一本安静的小笔记，保留原话与整理后的想法。");
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_count() == 0);
    assert(inspiration_storage_read(0, &out) == INSPIRATION_INVALID);
    assert(inspiration_storage_add(&r) == INSPIRATION_OK);
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_read(0, &out) == INSPIRATION_OK);
    assert(memcmp(&r, &out, sizeof(r)) == 0);
    /* Partial final append after a power interruption must not erase the good record. */
    FILE *f = fopen(path, "ab"); assert(f); assert(fwrite("partial", 1, 7, f) == 7); fclose(f);
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_count() == 1);
    struct stat st; assert(stat(path, &st) == 0 && st.st_size == INSPIRATION_RECORD_BYTES);
    /* Payload flushed but commit header absent: recover the slot without publishing it. */
    f = fopen(path, "ab"); assert(f);
    const unsigned char pending[12] = {0};
    assert(fwrite(pending, 1, 12, f) == 12);
    assert(fwrite(&r, 1, sizeof(r), f) == sizeof(r)); fclose(f);
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_count() == 1);
    assert(inspiration_storage_add(&r) == INSPIRATION_OK);
    assert(inspiration_storage_count() == 2);
    /* Three-byte Chinese / four-byte emoji boundaries, invalid trailing UTF-8. */
    char small[8]; inspiration_text_copy(small, sizeof(small), "想🙂法"); assert(strcmp(small, "想🙂") == 0);
    inspiration_text_copy(small, 3, "想"); assert(!small[0]);
    inspiration_text_copy(small, sizeof(small), "abc\xf0\x80\x80\x80"); assert(strcmp(small, "abc") == 0);
    r.text[0] = 0; assert(inspiration_storage_add(&r) == INSPIRATION_INVALID);
    strcpy(r.text, "body");
    for (unsigned i = inspiration_storage_count(); i < INSPIRATION_LIMIT; ++i) {
        snprintf(r.title, sizeof(r.title), "idea %u", i);
        assert(inspiration_storage_add(&r) == INSPIRATION_OK);
    }
    assert(inspiration_storage_add(&r) == INSPIRATION_FULL);
    assert(inspiration_storage_count() == INSPIRATION_LIMIT);
    assert(inspiration_storage_delete(250) == INSPIRATION_OK);
    strcpy(r.title, "Newest after slot reuse");
    assert(inspiration_storage_add(&r) == INSPIRATION_OK);
    assert(inspiration_storage_read(0, &out) == INSPIRATION_OK);
    assert(strcmp(out.title, r.title) == 0);
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_count() == INSPIRATION_LIMIT);
    assert(inspiration_storage_read(0, &out) == INSPIRATION_OK && strcmp(out.title, r.title) == 0);
    assert(stat(path, &st) == 0 && st.st_size == INSPIRATION_LIMIT * INSPIRATION_RECORD_BYTES);
    /* Corrupt a full slot: retain every other record, reuse the damaged slot. */
    f = fopen(path, "r+b"); assert(f); assert(fseek(f, 12 + 20 + 64 + 3, SEEK_SET) == 0);
    fputc('X', f); fclose(f);
    assert(inspiration_storage_init(path) == INSPIRATION_OK);
    assert(inspiration_storage_count() == INSPIRATION_LIMIT - 1);
    assert(inspiration_storage_add(&r) == INSPIRATION_OK);
    /* Failed writes / bad path must never be reported as saved. */
    assert(inspiration_storage_init("/no-such-directory/reppo/ideas.dat") == INSPIRATION_IO);
    assert(inspiration_storage_add(&r) == INSPIRATION_IO);
    puts("inspiration storage: reboot, UTF-8, CRC recovery, full capacity, deletion and reuse passed");
    return 0;
}
