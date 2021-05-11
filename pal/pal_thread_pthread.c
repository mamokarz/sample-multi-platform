// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include <pthread.h>
#include <time.h>
#include "pal.h"

void pal_sleep(unsigned int sleep_time_ms)
{
    time_t seconds = sleep_time_ms / 1000;
    long remainder_nanoseconds = (sleep_time_ms % 1000) * 1000000;
    struct timespec time_to_sleep = { seconds, remainder_nanoseconds };
    (void)nanosleep(&time_to_sleep, NULL);
}

