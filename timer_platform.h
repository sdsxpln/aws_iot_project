/*
 * Copyright 2015-2016 Texas Instruments Incorporated. All Rights Reserved.
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

#ifndef PLATFORM_TIRTOS_TIMER_PLATFORM_H_
#define PLATFORM_TIRTOS_TIMER_PLATFORM_H_

#include <stdint.h>

/*
 *  Cap timeout limits to prevent multiplication operations that could result
 *  in a uint32_t wrap
 */
#define TIMER_MAX_TIMEOUT_MS 4000000 
#define TIMER_MAX_TIMEOUT_SEC 4000

struct Timer {
    uint32_t start;
    uint32_t length;
};

#endif
