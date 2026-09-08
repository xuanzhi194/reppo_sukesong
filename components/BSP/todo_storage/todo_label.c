#include "todo_label.h"

#include <ctype.h>
#include <string.h>

static int ascii_ieq(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;
        if (ca >= 'A' && ca <= 'Z') {
            ca = (unsigned char)(ca + 32);
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = (unsigned char)(cb + 32);
        }
        if (ca != cb) {
            return 0;
        }
    }
    return *a == *b;
}

const char *todo_label_token(uint8_t label)
{
    switch (label) {
    case TODO_LABEL_WORK:  return "work";
    case TODO_LABEL_STUDY: return "study";
    case TODO_LABEL_LIFE:  return "life";
    case TODO_LABEL_FUN:   return "fun";
    default:               return "";
    }
}

const char *todo_label_display_name(uint8_t label)
{
    switch (label) {
    case TODO_LABEL_WORK:  return "Work";
    case TODO_LABEL_STUDY: return "Study";
    case TODO_LABEL_LIFE:  return "Life";
    case TODO_LABEL_FUN:   return "Fun";
    default:               return "";
    }
}

uint8_t todo_label_from_int(int v)
{
    if (v >= TODO_LABEL_WORK && v <= TODO_LABEL_FUN) {
        return (uint8_t)v;
    }
    return TODO_LABEL_NONE;
}

uint8_t todo_label_from_gash_cat(int cat_idx)
{
    if (cat_idx >= 0 && cat_idx <= 3) {
        return (uint8_t)(TODO_LABEL_WORK + cat_idx);
    }
    return TODO_LABEL_NONE;
}

uint8_t todo_label_parse(const char *s)
{
    char buf[32];
    size_t n = 0;
    const char *p;
    int as_int = 0;
    int saw_digit = 0;

    if (s == NULL) {
        return TODO_LABEL_NONE;
    }
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    p = s;
    while (*p != '\0' && n + 1 < sizeof(buf)) {
        if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            break;
        }
        buf[n++] = *p++;
    }
    buf[n] = '\0';
    if (n == 0 || ascii_ieq(buf, "none") || ascii_ieq(buf, "null")) {
        return TODO_LABEL_NONE;
    }

    for (p = buf; *p != '\0'; p++) {
        if (!isdigit((unsigned char)*p)) {
            saw_digit = 0;
            as_int = 0;
            break;
        }
        saw_digit = 1;
        as_int = as_int * 10 + (*p - '0');
    }
    if (saw_digit) {
        return todo_label_from_int(as_int);
    }

    if (ascii_ieq(buf, "work") || strcmp(buf, "工作") == 0) {
        return TODO_LABEL_WORK;
    }
    if (ascii_ieq(buf, "study") || strcmp(buf, "学习") == 0) {
        return TODO_LABEL_STUDY;
    }
    if (ascii_ieq(buf, "life") || ascii_ieq(buf, "live") || strcmp(buf, "生活") == 0) {
        return TODO_LABEL_LIFE;
    }
    if (ascii_ieq(buf, "fun") || ascii_ieq(buf, "play") ||
        ascii_ieq(buf, "entertainment") || strcmp(buf, "娱乐") == 0) {
        return TODO_LABEL_FUN;
    }
    return TODO_LABEL_NONE;
}
