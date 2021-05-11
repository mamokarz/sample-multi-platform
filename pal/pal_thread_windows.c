// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// See LICENSE file in the project root for full license information.

#include <windows.h>
#include <time.h>
#include "pal.h"

void pal_sleep(unsigned int sleep_time_ms)
{
    Sleep(sleep_time_ms);
}

