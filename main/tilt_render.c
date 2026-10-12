/* Code-drawn selector and tilt-orb scenes. No Guider assets, timers or heap. */
#include "tilt_render.h"
#include <stdio.h>

#define NAVY     0x102033
#define DEEP     0x0B1724
#define CARD     0x1B3A56
#define CARD_ON  0x274A6E
#define TEAL     0x5EC8C5
#define TEAL_DIM 0x2A6F72
#define PAPER    0xF4F1E8
#define INK      0xE8F0F4
#define MUTED    0x8AA0B5
#define LINE     0x1E4A6E
#define GOLD     0xE8C36A
#define CORAL    0xE07A5F

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
        if (*value == ' ') {
            x += scale * 3;
            continue;
        }
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

static int clampi(int value, int lo, int hi)
{
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static void orb(lv_draw_ctx_t *ctx, const lv_area_t *a, int x, int y,
                int radius, uint32_t phase_age)
{
    int pulse = ((int)(phase_age / 80) % 6);
    box(ctx, a, x - radius - 5, y - radius - 5, radius * 2 + 10, radius * 2 + 10,
        TEAL, radius + 5, LV_OPA_20);
    box(ctx, a, x - radius - 2, y - radius - 2, radius * 2 + 4, radius * 2 + 4,
        TEAL, radius + 2, LV_OPA_50);
    box(ctx, a, x - radius, y - radius, radius * 2, radius * 2,
        TEAL, radius, LV_OPA_COVER);
    box(ctx, a, x - radius + 3, y - radius + 2, radius * 2 - 7, radius * 2 - 7,
        0xB8F3EE, radius - 2, LV_OPA_COVER);
    box(ctx, a, x - radius + 5, y - radius + 3, 5 + pulse / 2, 4, PAPER, 2, LV_OPA_COVER);
}

static void mini_dino(lv_draw_ctx_t *ctx, const lv_area_t *a, int x, int y)
{
    static const char *const sprite[8] = {
        "...####.", "..#####.", "..##.##.", "..####..",
        ".#####..", "######.#", ".#####..", "..#..#..",
    };
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            if (sprite[row][col] == '#') {
                box(ctx, a, x + col * 3, y + row * 3, 3, 3, PAPER, 0, LV_OPA_COVER);
            }
        }
    }
}

static void overlay(lv_draw_ctx_t *ctx, const lv_area_t *a, uint8_t selection,
                    const char *heading, const char *hint, const char *action,
                    int heading_width)
{
    box(ctx, a, 86, 46, 228, 84, DEEP, 8, LV_OPA_80);
    box(ctx, a, 90, 50, 220, 76, CARD, 7, LV_OPA_COVER);
    title(ctx, a, 90 + (220 - heading_width) / 2, 56, heading, 2, PAPER);
    text(ctx, a, 96, 76, 208, hint, MUTED, LV_TEXT_ALIGN_CENTER);
    box(ctx, a, 102, 96, 92, 24, selection == 0 ? TEAL : LINE, 5, LV_OPA_COVER);
    box(ctx, a, 202, 96, 96, 24, selection == 1 ? TEAL : LINE, 5, LV_OPA_COVER);
    text(ctx, a, 102, 99, 92, action, selection == 0 ? DEEP : INK, LV_TEXT_ALIGN_CENTER);
    text(ctx, a, 202, 99, 96, "HOME", selection == 1 ? DEEP : INK, LV_TEXT_ALIGN_CENTER);
}

void tilt_render(lv_draw_ctx_t *ctx, const lv_area_t *a,
                 const tilt_game_t *game, uint8_t selection,
                 uint32_t phase_age_ms, uint32_t milestone_ms)
{
    int shake = 0;
    int ox;
    int oy;
    char score[32];
    int drift = (int)game->distance_px % 220;

    if (game->phase == TILT_PHASE_GAME_OVER && phase_age_ms < 280) {
        shake = (int)((280 - phase_age_ms) / 40) * ((phase_age_ms / 30) & 1 ? 1 : -1);
    }
    box(ctx, a, 0, 0, 320, 170, DEEP, 0, LV_OPA_COVER);
    box(ctx, a, 0, 0, 320, TILT_PLAY_TOP - 2, NAVY, 0, LV_OPA_COVER);
    box(ctx, a, 0, TILT_PLAY_BOTTOM + 1, 320, 170 - TILT_PLAY_BOTTOM - 1,
        NAVY, 0, LV_OPA_COVER);

    for (int i = 0; i < 14; ++i) {
        int sx = (47 * i + 310 - drift * (1 + (i & 1))) % 330 - 6;
        int sy = 38 + (i * 17 + (i * 3) % 11) % 100;
        box(ctx, a, sx, sy, 2, 2, i & 1 ? TEAL_DIM : MUTED, 1, LV_OPA_60);
    }
    box(ctx, a, 0, TILT_PLAY_TOP - 1, 320, 1, TEAL_DIM, 0, LV_OPA_COVER);
    box(ctx, a, 0, TILT_PLAY_BOTTOM, 320, 1, TEAL_DIM, 0, LV_OPA_COVER);
    box(ctx, a, 0, (TILT_PLAY_TOP + TILT_PLAY_BOTTOM) / 2, 320, 1, LINE, 0, LV_OPA_20);
    for (int x = 8; x < 320; x += 28) {
        box(ctx, a, x, TILT_PLAY_TOP + 4, 12, 1, TEAL_DIM, 0, LV_OPA_20);
        box(ctx, a, x + 6, TILT_PLAY_BOTTOM - 5, 12, 1, TEAL_DIM, 0, LV_OPA_20);
    }

    title(ctx, a, 12, 8, "TILT ORB", 1, TEAL);
    snprintf(score, sizeof(score), "HI %05lu   %05lu",
             (unsigned long)(game->best > 99999 ? 99999 : game->best),
             (unsigned long)(game->score > 99999 ? 99999 : game->score));
    text(ctx, a, 143, 5, 165, score, milestone_ms ? GOLD : PAPER, LV_TEXT_ALIGN_RIGHT);

    for (int i = 0; i < TILT_MAX_GATES; ++i) {
        const tilt_gate_t *gate = &game->gates[i];
        int x;
        int gap_top;
        int gap_bot;
        if (!gate->active) continue;
        x = (int)gate->x + shake;
        gap_top = (int)(gate->gap_y - gate->gap_h * 0.5f);
        gap_bot = (int)(gate->gap_y + gate->gap_h * 0.5f);
        box(ctx, a, x, TILT_PLAY_TOP, gate->width, gap_top - TILT_PLAY_TOP,
            LINE, 3, LV_OPA_COVER);
        box(ctx, a, x, gap_bot, gate->width, TILT_PLAY_BOTTOM - gap_bot,
            LINE, 3, LV_OPA_COVER);
        box(ctx, a, x, gap_top - 3, gate->width, 3, TEAL, 1, LV_OPA_COVER);
        box(ctx, a, x, gap_bot, gate->width, 3, TEAL, 1, LV_OPA_COVER);
        box(ctx, a, x + 2, TILT_PLAY_TOP + 3, 2, gap_top - TILT_PLAY_TOP - 8,
            TEAL, 0, LV_OPA_30);
    }
    for (int i = 0; i < TILT_MAX_STARS; ++i) {
        const tilt_star_t *star = &game->pickups[i];
        int x;
        int y;
        if (!star->active) continue;
        x = (int)star->x + shake;
        y = (int)star->y;
        box(ctx, a, x - 5, y - 5, 10, 10, GOLD, 5, LV_OPA_30);
        box(ctx, a, x - 3, y - 3, 6, 6, GOLD, 3, LV_OPA_COVER);
        box(ctx, a, x - 1, y - 1, 2, 2, PAPER, 1, LV_OPA_COVER);
    }

    ox = (int)game->orb_x + shake;
    oy = (int)game->orb_y;
    if (game->phase == TILT_PHASE_RUNNING) {
        box(ctx, a, ox - (int)(game->vx * 0.06f) - 4,
            oy - (int)(game->vy * 0.06f) - 4, 8, 8, TEAL, 4, LV_OPA_30);
        box(ctx, a, ox - (int)(game->vx * 0.12f) - 3,
            oy - (int)(game->vy * 0.12f) - 3, 6, 6, TEAL_DIM, 3, LV_OPA_40);
    }
    orb(ctx, a, ox, oy, 8, phase_age_ms);
    if (game->phase == TILT_PHASE_GAME_OVER && phase_age_ms < 500) {
        int spread = 4 + (int)(phase_age_ms / 40);
        box(ctx, a, ox + spread, oy - 2, 3, 3, CORAL, 1, LV_OPA_COVER);
        box(ctx, a, ox - spread, oy + 3, 2, 2, GOLD, 1, LV_OPA_COVER);
        box(ctx, a, ox + 2, oy - spread, 3, 2, PAPER, 1, LV_OPA_COVER);
    }

    if (game->phase != TILT_PHASE_RUNNING) {
        const char *heading = "TILT ORB";
        const char *hint = "Tilt to steer through the gates.";
        const char *action = "START";
        int heading_width = 94;
        if (game->phase == TILT_PHASE_PAUSED) {
            heading = "PAUSED"; hint = "Keep the orb in the glow.";
            action = "RESUME"; heading_width = 70;
        } else if (game->phase == TILT_PHASE_GAME_OVER) {
            heading = "GAME OVER"; hint = "One more orbit?";
            action = "RETRY"; heading_width = 106;
        }
        overlay(ctx, a, selection, heading, hint, action, heading_width);
    } else if (milestone_ms) {
        text(ctx, a, 106, 36, 110, "NICE ORBIT!", GOLD, LV_TEXT_ALIGN_CENTER);
    }

    box(ctx, a, 0, 153, 320, 17, 0x0E1B28, 0, LV_OPA_COVER);
    text(ctx, a, 0, 153, 320,
         game->phase == TILT_PHASE_RUNNING ?
         "TILT steer   /   TURN pause" : "CLICK select   /   TURN select",
         TEAL, LV_TEXT_ALIGN_CENTER);
}

void game_select_render(lv_draw_ctx_t *ctx, const lv_area_t *a,
                        uint8_t catalog, uint8_t selection,
                        float tilt_x, uint32_t dino_best, uint32_t tilt_best,
                        uint32_t phase_age_ms)
{
    int bead;
    int lift0 = catalog == 0 ? 0 : 4;
    int lift1 = catalog == 1 ? 0 : 4;
    char left[24];
    char right[24];
    int shimmer = (int)((phase_age_ms / 90) % 12);

    if (tilt_x > 1.0f) tilt_x = 1.0f;
    if (tilt_x < -1.0f) tilt_x = -1.0f;
    bead = 156 + (int)(tilt_x * 78.0f);
    bead = clampi(bead, 78, 234);

    box(ctx, a, 0, 0, 320, 170, DEEP, 0, LV_OPA_COVER);
    box(ctx, a, 0, 0, 320, 28, NAVY, 0, LV_OPA_COVER);
    title(ctx, a, 12, 8, "PLAY", 2, PAPER);
    text(ctx, a, 160, 7, 148, "tilt to switch", MUTED, LV_TEXT_ALIGN_RIGHT);

    box(ctx, a, 14 - (catalog == 0 ? 1 : 0), 34 + lift0,
        142 + (catalog == 0 ? 2 : 0), 88 - lift0,
        catalog == 0 ? TEAL : CARD, 10, catalog == 0 ? LV_OPA_COVER : LV_OPA_70);
    box(ctx, a, 18, 38 + lift0, 134, 80 - lift0,
        catalog == 0 ? CARD_ON : CARD, 8, LV_OPA_COVER);
    mini_dino(ctx, a, 64, 46 + lift0);
    title(ctx, a, 36, 74 + lift0, "DINO RUN", 1, catalog == 0 ? PAPER : MUTED);
    snprintf(left, sizeof(left), "HI %lu",
             (unsigned long)(dino_best > 99999 ? 99999 : dino_best));
    text(ctx, a, 18, 90 + lift0, 134, left, catalog == 0 ? GOLD : MUTED,
         LV_TEXT_ALIGN_CENTER);

    box(ctx, a, 164 - (catalog == 1 ? 1 : 0), 34 + lift1,
        142 + (catalog == 1 ? 2 : 0), 88 - lift1,
        catalog == 1 ? TEAL : CARD, 10, catalog == 1 ? LV_OPA_COVER : LV_OPA_70);
    box(ctx, a, 168, 38 + lift1, 134, 80 - lift1,
        catalog == 1 ? CARD_ON : CARD, 8, LV_OPA_COVER);
    orb(ctx, a, 235, 58 + lift1, 9, phase_age_ms);
    title(ctx, a, 186, 74 + lift1, "TILT ORB", 1, catalog == 1 ? PAPER : MUTED);
    snprintf(right, sizeof(right), "HI %lu",
             (unsigned long)(tilt_best > 99999 ? 99999 : tilt_best));
    text(ctx, a, 168, 90 + lift1, 134, right, catalog == 1 ? GOLD : MUTED,
         LV_TEXT_ALIGN_CENTER);

    if (catalog == 0) {
        box(ctx, a, 22 + shimmer, 40, 18, 2, TEAL, 1, LV_OPA_40);
    } else {
        box(ctx, a, 172 + shimmer, 40, 18, 2, TEAL, 1, LV_OPA_40);
    }

    box(ctx, a, 70, 126, 180, 10, 0x0E1B28, 5, LV_OPA_COVER);
    box(ctx, a, 72, 128, 176, 6, LINE, 3, LV_OPA_COVER);
    box(ctx, a, 156, 127, 8, 8, TEAL_DIM, 4, LV_OPA_40);
    box(ctx, a, bead - 6, 125, 12, 12, PAPER, 6, LV_OPA_COVER);
    box(ctx, a, bead - 3, 128, 6, 6, TEAL, 3, LV_OPA_COVER);

    box(ctx, a, 12, 142, 88, 22, selection == 0 ? TEAL : CARD, 5, LV_OPA_COVER);
    box(ctx, a, 220, 142, 88, 22, selection == 1 ? TEAL : CARD, 5, LV_OPA_COVER);
    text(ctx, a, 12, 144, 88, "PLAY", selection == 0 ? DEEP : INK, LV_TEXT_ALIGN_CENTER);
    text(ctx, a, 220, 144, 88, "HOME", selection == 1 ? DEEP : INK, LV_TEXT_ALIGN_CENTER);
}
