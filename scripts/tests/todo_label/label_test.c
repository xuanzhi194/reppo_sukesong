#include "todo_label.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_tokens(void)
{
    assert(strcmp(todo_label_token(TODO_LABEL_NONE), "") == 0);
    assert(strcmp(todo_label_token(TODO_LABEL_WORK), "work") == 0);
    assert(strcmp(todo_label_token(TODO_LABEL_STUDY), "study") == 0);
    assert(strcmp(todo_label_token(TODO_LABEL_LIFE), "life") == 0);
    assert(strcmp(todo_label_token(TODO_LABEL_FUN), "fun") == 0);
    assert(strcmp(todo_label_display_name(TODO_LABEL_WORK), "Work") == 0);
    assert(strcmp(todo_label_display_name(TODO_LABEL_NONE), "") == 0);
}

static void test_parse_english(void)
{
    assert(todo_label_parse(NULL) == TODO_LABEL_NONE);
    assert(todo_label_parse("") == TODO_LABEL_NONE);
    assert(todo_label_parse("none") == TODO_LABEL_NONE);
    assert(todo_label_parse("  ") == TODO_LABEL_NONE);
    assert(todo_label_parse("work") == TODO_LABEL_WORK);
    assert(todo_label_parse("WORK") == TODO_LABEL_WORK);
    assert(todo_label_parse(" Work ") == TODO_LABEL_WORK);
    assert(todo_label_parse("study") == TODO_LABEL_STUDY);
    assert(todo_label_parse("life") == TODO_LABEL_LIFE);
    assert(todo_label_parse("fun") == TODO_LABEL_FUN);
    assert(todo_label_parse("play") == TODO_LABEL_FUN);
    assert(todo_label_parse("mystery") == TODO_LABEL_NONE);
}

static void test_parse_chinese(void)
{
    assert(todo_label_parse("工作") == TODO_LABEL_WORK);
    assert(todo_label_parse("学习") == TODO_LABEL_STUDY);
    assert(todo_label_parse("生活") == TODO_LABEL_LIFE);
    assert(todo_label_parse("娱乐") == TODO_LABEL_FUN);
}

static void test_parse_numbers(void)
{
    assert(todo_label_parse("0") == TODO_LABEL_NONE);
    assert(todo_label_parse("1") == TODO_LABEL_WORK);
    assert(todo_label_parse("2") == TODO_LABEL_STUDY);
    assert(todo_label_parse("3") == TODO_LABEL_LIFE);
    assert(todo_label_parse("4") == TODO_LABEL_FUN);
    assert(todo_label_parse("5") == TODO_LABEL_NONE);
    assert(todo_label_from_int(0) == TODO_LABEL_NONE);
    assert(todo_label_from_int(2) == TODO_LABEL_STUDY);
    assert(todo_label_from_gash_cat(0) == TODO_LABEL_WORK);
    assert(todo_label_from_gash_cat(3) == TODO_LABEL_FUN);
    assert(todo_label_from_gash_cat(4) == TODO_LABEL_NONE);
    assert(todo_label_from_gash_cat(-1) == TODO_LABEL_NONE);
}

static void test_roundtrip(void)
{
    uint8_t i;
    for (i = TODO_LABEL_NONE; i <= TODO_LABEL_FUN; i++) {
        assert(todo_label_parse(todo_label_token(i)) == i);
    }
}

int main(int argc, char **argv)
{
    const char *name = argc > 1 ? argv[1] : "all";
    if (strcmp(name, "all") == 0 || strcmp(name, "tokens") == 0) test_tokens();
    if (strcmp(name, "all") == 0 || strcmp(name, "english") == 0) test_parse_english();
    if (strcmp(name, "all") == 0 || strcmp(name, "chinese") == 0) test_parse_chinese();
    if (strcmp(name, "all") == 0 || strcmp(name, "numbers") == 0) test_parse_numbers();
    if (strcmp(name, "all") == 0 || strcmp(name, "roundtrip") == 0) test_roundtrip();
    puts("PASS");
    return 0;
}
