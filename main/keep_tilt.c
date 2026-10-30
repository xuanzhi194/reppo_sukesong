#include "keep_tilt.h"
#include <math.h>
#include <string.h>

#define ENTER_DEG 6.0f
#define RELEASE_DEG 4.0f
#define REPEAT_MS 600u

void keep_tilt_reset(keep_tilt_t *s) { memset(s, 0, sizeof(*s)); }

int keep_tilt_update(keep_tilt_t *s, const imu_sample_t *sample)
{
    if (!sample || !sample->valid) {
        s->valid = false;
        s->direction = 0;
        return 0;
    }
    if (s->seen && sample->sequence == s->sequence) return 0;
    if (s->seen && sample->timestamp_ms - s->sample_at > 250) s->direction = 0;
    s->seen = true;
    s->sequence = sample->sequence;
    s->sample_at = sample->timestamp_ms;
    float x = sample->accel_g[0], y = sample->accel_g[1], z = sample->accel_g[2];
    float mag2 = x*x + y*y + z*z;
    if (!(mag2 > 0.65f*0.65f && mag2 < 1.35f*1.35f)) {
        s->valid = false;
        s->direction = 0;
        return 0;
    }
    s->valid = true;
    /* Absolute X gravity angle; no entry calibration or return-to-center gate. */
    float angle = atan2f(x, sqrtf(y*y + z*z)) * 57.2957795f;
    int direction = angle >= ENTER_DEG ? 1 : angle <= -ENTER_DEG ? -1 : 0;
    /* A small dead zone prevents threshold chatter; it never blocks reversal. */
    if (!direction && s->direction && angle * s->direction > RELEASE_DEG)
        direction = s->direction;
    if (!direction) { s->direction = 0; return 0; }
    if (direction != s->direction || sample->timestamp_ms - s->fired_at >= REPEAT_MS) {
        s->direction = direction;
        s->fired_at = sample->timestamp_ms;
        return direction;
    }
    return 0;
}
