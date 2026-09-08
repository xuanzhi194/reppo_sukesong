/* Small code-drawn sprites: independent of GUI Guider's exported image assets. */
#include "dino_render.h"
#include <stdio.h>

#define PAPER  0xF5F5EB
#define INK    0x293C34
#define GREEN  0x397A56
#define MUTED  0x7D8C7B
#define LINE   0xC9D2C2
#define GOLD   0xAB792F

static void box(lv_draw_ctx_t *ctx, const lv_area_t *origin, int x, int y,
                int w, int h, uint32_t color, int radius, lv_opa_t opacity)
{
    if (w <= 0 || h <= 0) return;
    lv_area_t area = {origin->x1 + x, origin->y1 + y,
                      origin->x1 + x + w - 1, origin->y1 + y + h - 1};
    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = lv_color_hex(color);
    dsc.bg_opa = opacity;
    dsc.radius = radius;
    lv_draw_rect(ctx, &dsc, &area);
}

static void text(lv_draw_ctx_t *ctx, const lv_area_t *origin, int x, int y,
                 int w, const char *value, uint32_t color, lv_text_align_t align)
{
    lv_draw_label_dsc_t dsc;
    lv_draw_label_dsc_init(&dsc);
    dsc.font = LV_FONT_DEFAULT;
    dsc.color = lv_color_hex(color);
    dsc.align = align;
    lv_area_t area = {origin->x1 + x, origin->y1 + y,
                      origin->x1 + x + w - 1, origin->y1 + y + 18};
    lv_draw_label(ctx, &dsc, &area, value, NULL);
}

/* 5x7 title glyphs, so the game does not depend on exported font names. */
static const uint8_t *glyph(char ch)
{
    static const uint8_t letters[26][7] = {
        {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
        {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
        {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
        {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
        {31,4,4,4,4,4,31},    {7,2,2,2,18,18,12},
        {17,18,20,24,20,18,17},{16,16,16,16,16,16,31},
        {17,27,21,21,17,17,17},{17,25,21,19,17,17,17},
        {14,17,17,17,17,17,14},{30,17,17,30,16,16,16},
        {14,17,17,17,21,18,13},{30,17,17,30,20,18,17},
        {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
        {17,17,17,17,17,17,14},{17,17,17,17,17,10,4},
        {17,17,17,21,21,21,10},{17,17,10,4,10,17,17},
        {17,17,10,4,4,4,4},   {31,1,2,4,8,16,31},
    };
    return ch >= 'A' && ch <= 'Z' ? letters[ch - 'A'] : NULL;
}

static void title(lv_draw_ctx_t *ctx, const lv_area_t *origin,
                  int x, int y, const char *value, int scale, uint32_t color)
{
    for (; *value; ++value, x += scale * 6) {
        const uint8_t *g = glyph(*value);
        if (g == NULL) continue;
        for (int row = 0; row < 7; ++row) {
            for (int col = 0; col < 5; ++col) {
                if (g[row] & (1 << (4 - col))) {
                    box(ctx, origin, x + col * scale, y + row * scale,
                        scale, scale, color, 0, LV_OPA_COVER);
                }
            }
        }
    }
}

static void cloud(lv_draw_ctx_t *ctx, const lv_area_t *a, int x, int y)
{
    box(ctx, a, x + 6, y, 15, 7, LINE, 3, LV_OPA_COVER);
    box(ctx, a, x, y + 5, 31, 5, LINE, 2, LV_OPA_COVER);
    box(ctx, a, x + 3, y + 5, 25, 3, PAPER, 0, LV_OPA_COVER);
}

static void dinosaur(lv_draw_ctx_t *ctx, const lv_area_t *a,
                     const dino_game_t *game, uint32_t phase_age)
{
    static const char *const sprite[15] = {
        "........######", ".......#######", ".......##.####",
        ".......#######", ".......####...", ".......######.",
        "#.....####....", "##...#####....", "###.########..",
        "##########..#.", ".#########....", "..#######.....",
        "...###.##.....", "...##...##....", "...###..###...",
    };
    int y = DINO_GROUND_Y - DINO_HEIGHT - (int)game->jump_y;
    uint32_t frame = (uint32_t)(game->distance_px / 9.0f) & 1U;
    int shadow = 23 - (int)(game->jump_y / 6.0f);
    box(ctx, a, DINO_X + (DINO_WIDTH - shadow) / 2, DINO_GROUND_Y + 2,
        shadow, 3, LINE, 2, LV_OPA_70);
    for (int row = 0; row < 15; ++row) {
        const char *pixels = sprite[row];
        if (game->phase == DINO_PHASE_RUNNING && game->jump_y == 0 && row >= 13) {
            pixels = row == 13 ? (frame ? "...##...#....." : "....#...##....") :
                                (frame ? "...###........" : "........###...");
        }
        int col = 0;
        while (col < 14) {
            if (pixels[col] != '#') { ++col; continue; }
            int start = col;
            while (col < 14 && pixels[col] == '#') ++col;
            box(ctx, a, DINO_X + start * 2, y + row * 2,
                (col - start) * 2, 2, INK, 0, LV_OPA_COVER);
        }
    }
    if (game->phase == DINO_PHASE_GAME_OVER) {
        box(ctx, a, DINO_X + 18, y + 4, 3, 3, PAPER, 0, LV_OPA_COVER);
        if (phase_age < 500) {
            int spread = 3 + (int)(phase_age / 45);
            box(ctx, a, DINO_X + 29 + spread, y + 9 - spread, 3, 3, GOLD, 0, LV_OPA_COVER);
            box(ctx, a, DINO_X + 31 + spread, y + 16, 2, 2, GOLD, 0, LV_OPA_COVER);
            box(ctx, a, DINO_X + 28, y - spread, 2, 3, GOLD, 0, LV_OPA_COVER);
        }
    }
}

static void cactus(lv_draw_ctx_t *ctx, const lv_area_t *a, int x, int width, int height)
{
    int stem = width >= 20 ? 6 : 4;
    int center = x + (width - stem) / 2;
    int top = DINO_GROUND_Y - height;
    box(ctx, a, center, top, stem, height, GREEN, 2, LV_OPA_COVER);
    box(ctx, a, x, top + 7, 3, height / 3, GREEN, 1, LV_OPA_COVER);
    box(ctx, a, x, top + 7 + height / 3 - 3, center - x + 2, 3, GREEN, 0, LV_OPA_COVER);
    box(ctx, a, x + width - 3, top + 12, 3, height / 3, GREEN, 1, LV_OPA_COVER);
    box(ctx, a, center + stem - 1, top + 12 + height / 3 - 3,
        x + width - center - stem + 1, 3, GREEN, 0, LV_OPA_COVER);
    box(ctx, a, center + 1, top + 4, 1, height - 7, PAPER, 0, LV_OPA_30);
}

void dino_render(lv_draw_ctx_t *ctx, const lv_area_t *a,
                 const dino_game_t *game, uint8_t selection,
                 uint32_t phase_age_ms, uint32_t milestone_ms)
{
    box(ctx, a, 0, 0, 320, 170, PAPER, 0, LV_OPA_COVER);
    title(ctx, a, 12, 12, "REPPO DINO", 1, GREEN);
    char score[32];
    snprintf(score, sizeof(score), "HI %05lu   %05lu",
             (unsigned long)(game->best > 99999 ? 99999 : game->best),
             (unsigned long)(game->score > 99999 ? 99999 : game->score));
    text(ctx, a, 143, 7, 165, score, milestone_ms ? GOLD : INK, LV_TEXT_ALIGN_RIGHT);
    box(ctx, a, 12, 29, 296, 1, LINE, 0, LV_OPA_COVER);

    int drift = (int)(game->distance_px / 10.0f) % 390;
    cloud(ctx, a, 98 - drift, 47);
    cloud(ctx, a, 310 - drift, 65);
    cloud(ctx, a, 488 - drift, 47);
    box(ctx, a, 273, 42, 13, 13, 0xDEDDBA, 7, LV_OPA_COVER);
    box(ctx, a, 0, DINO_GROUND_Y, 320, 1, MUTED, 0, LV_OPA_COVER);
    int offset = (int)game->distance_px % 61;
    for (int x = -61; x < 381; x += 61) {
        box(ctx, a, x - offset, 144, 10, 1, LINE, 0, LV_OPA_COVER);
        box(ctx, a, x + 26 - offset, 148, 3, 1, LINE, 0, LV_OPA_COVER);
    }
    for (int i = 0; i < DINO_MAX_OBSTACLES; ++i) {
        const dino_obstacle_t *o = &game->obstacles[i];
        if (o->active && o->x < 321 && o->x + o->width > 0) {
            cactus(ctx, a, (int)o->x, o->width, o->height);
        }
    }
    dinosaur(ctx, a, game, phase_age_ms);

    if (game->phase != DINO_PHASE_RUNNING) {
        const char *heading = "DINO RUN";
        const char *hint = "Jump over the cacti.";
        const char *action = "START";
        int heading_width = 94;
        if (game->phase == DINO_PHASE_PAUSED) {
            heading = "PAUSED"; hint = "Take a little breather.";
            action = "RESUME"; heading_width = 70;
        } else if (game->phase == DINO_PHASE_GAME_OVER) {
            heading = "GAME OVER"; hint = "One more little adventure?";
            action = "RETRY"; heading_width = 106;
        }
        box(ctx, a, 99, 50, 209, 81, PAPER, 6, LV_OPA_COVER);
        title(ctx, a, 99 + (209 - heading_width) / 2, 57, heading, 2, INK);
        text(ctx, a, 100, 78, 207, hint, MUTED, LV_TEXT_ALIGN_CENTER);
        box(ctx, a, 109, 102, 88, 24, selection == 0 ? GREEN : LINE, 5, LV_OPA_COVER);
        box(ctx, a, 204, 102, 94, 24, selection == 1 ? GREEN : LINE, 5, LV_OPA_COVER);
        text(ctx, a, 109, 105, 88, action, selection == 0 ? PAPER : INK, LV_TEXT_ALIGN_CENTER);
        text(ctx, a, 204, 105, 94, "HOME", selection == 1 ? PAPER : INK, LV_TEXT_ALIGN_CENTER);
    } else if (milestone_ms) {
        text(ctx, a, 106, 34, 110, "NICE RUN!", GOLD, LV_TEXT_ALIGN_CENTER);
    }

    box(ctx, a, 0, 153, 320, 17, 0xE7ECDD, 0, LV_OPA_COVER);
    const char *footer = game->phase == DINO_PHASE_RUNNING ?
        "CLICK jump   /   TURN pause" : "CLICK select   /   TURN select";
    text(ctx, a, 0, 153, 320, footer, GREEN, LV_TEXT_ALIGN_CENTER);
}
