#include "keep_tilt.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

static keep_tilt_t state;
static imu_sample_t sample;
static unsigned right_events, left_events;
static void reset(uint32_t time)
{
    keep_tilt_reset(&state);
    sample = (imu_sample_t){.timestamp_ms = time};
    right_events = left_events = 0;
}
static int feed(float degrees, float magnitude, float pitch, unsigned dt, unsigned repeats)
{
    sample.valid = true;
    sample.sequence++;
    sample.timestamp_ms += dt;
    float a = degrees * 0.01745329252f, p = pitch * 0.01745329252f;
    sample.accel_g[0] = sinf(a) * magnitude;
    sample.accel_g[1] = cosf(a) * sinf(p) * magnitude;
    sample.accel_g[2] = cosf(a) * cosf(p) * magnitude;
    int event = keep_tilt_update(&state, &sample);
    if (event > 0) ++right_events;
    if (event < 0) ++left_events;
    for (unsigned i = 0; i < repeats; ++i) assert(keep_tilt_update(&state, &sample) == 0);
    return event;
}
static void hold(float angle, unsigned samples)
{ for (unsigned i = 0; i < samples; ++i) feed(angle, 1, 0, 50, 30); }
int main(void)
{
    reset(0);
    assert(feed(10, 1, 0, 50, 100) == 1); /* First sample, no calibration. */
    assert(feed(-10, 1, 0, 50, 100) == -1); /* Immediate reversal. */
    for (unsigned i = 0; i < 100; ++i) {
        assert(feed(10, 1, 0, 50, 20) == 1);
        assert(feed(-10, 1, 0, 50, 20) == -1);
    }
    puts("PASS first-frame switching and 100 immediate direction reversals without centering");

    reset(0);
    assert(feed(10, 1, 0, 50, 100) == 1);
    hold(10, 11); assert(right_events == 1); /* 550 ms, no repeat yet. */
    hold(10, 1); assert(right_events == 2);  /* Exactly 600 ms. */
    hold(10, 120); assert(right_events == 12);
    assert(feed(-10, 1, 0, 50, 100) == -1);
    puts("PASS held tilt repeats every 600ms, duplicate frames do not repeat, reversal bypasses timer");

    reset(0);
    for (unsigned i = 0; i < 200; ++i)
        feed((int)(i % 7) - 3, 0.8f + (i % 5)*0.1f, (int)(i % 120) - 60, 50, 4);
    assert(!right_events && !left_events);
    assert(feed(7, 1, 0, 50, 10) == 1);
    for (unsigned i = 0; i < 8; ++i) feed(i % 2 ? 5 : 6.5f, 1, 0, 50, 10);
    assert(right_events == 1); /* Boundary jitter doesn't act like new gestures. */
    assert(feed(0, 1, 0, 50, 10) == 0);
    assert(feed(7, 1, 0, 50, 10) == 1);
    puts("PASS resting dead zone, varying pitch/gravity, threshold hysteresis and re-tilt");

    reset(0);
    assert(feed(20, 3.2f, 0, 50, 10) == 0);
    assert(feed(10, 1, 0, 50, 10) == 1); /* No recovery/centering gate. */
    sample.valid = false; assert(keep_tilt_update(&state, &sample) == 0);
    assert(feed(-10, 1, 0, 50, 10) == -1);
    assert(feed(10, 1, 0, 1000, 10) == 1);
    reset(UINT32_MAX - 100);
    assert(feed(10, 1, 0, 50, 100) == 1);
    hold(10, 12); assert(right_events == 2);
    puts("PASS invalid sample rejection, immediate recovery and timer wrap");
    return 0;
}
