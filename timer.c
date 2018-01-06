/*
 * Copyright 2015-2017 Texas Instruments Incorporated. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <assert.h>
#include <time.h>
#include <timer_interface.h>
#include "aws_iot_log.h"


uint32_t left_ms(Timer *timer)
{
    struct timespec currTime;
    uint32_t diff;
    uint32_t timeMs;
    uint32_t fractionMs;

    clock_gettime(CLOCK_MONOTONIC, &currTime);

    timeMs = currTime.tv_sec * 1000;
    fractionMs = currTime.tv_nsec / 1000000;

    /* compute time difference in milliseconds and check for expiration */
    diff = (timeMs + fractionMs) - timer->start;

    if (diff >= timer->length) {
        timer->length = 0;
        diff = 0;
    }
    else {
        diff = timer->length - diff;
    }

    return (diff);
}

bool has_timer_expired(Timer *timer)
{
    return (!left_ms(timer));
}

void countdown_ms(Timer *timer, uint32_t tms)
{
    struct timespec currTime;
    uint32_t timeMs;
    uint32_t fractionMs;

    if ( tms >= TIMER_MAX_TIMEOUT_MS ) {
        IOT_ERROR("Ooops ... ");
    }
    assert(tms < TIMER_MAX_TIMEOUT_MS);
    timer->length = tms;

    clock_gettime(CLOCK_MONOTONIC, &currTime);

    timeMs = currTime.tv_sec * 1000;
    fractionMs = currTime.tv_nsec / 1000000;

    /* save the start time in milliseconds */
    timer->start = timeMs + fractionMs;
}

void countdown_sec(Timer *timer, uint32_t tsec)
{
    assert(tsec < TIMER_MAX_TIMEOUT_SEC);
    countdown_ms(timer, tsec * 1000);
}

void init_timer(Timer *timer)
{
    timer->start = 0;
    timer->length = 0;
}
