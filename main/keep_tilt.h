#pragma once
#include "imu_motion.h"

/* Exhibition controls: immediate direction changes, timed repeats while held. */
typedef struct {
    bool seen, valid;
    uint32_t sequence, sample_at, fired_at;
    int direction;
} keep_tilt_t;

void keep_tilt_reset(keep_tilt_t *state);
/* -1 previous; +1 next; 0 unchanged. Only new hardware samples are consumed. */
int keep_tilt_update(keep_tilt_t *state, const imu_sample_t *sample);
