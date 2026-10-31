#pragma once
#include <stdbool.h>
#include <stddef.h>
/* Pure, bounded text handling shared by firmware and host tests. */
void boss_text_copy(char *out, size_t cap, const char *text);
bool boss_rules_translate(const char *source, char *out, size_t cap, char *effect, size_t effect_cap);

bool boss_parse_ai(const char *text, char *reply, size_t cap, char *effect, size_t effect_cap);
