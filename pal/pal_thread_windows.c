#include <windows.h>
#include <time.h>
#include "pal.h"

void pal_sleep(unsigned int sleep_time_ms)
{
    Sleep(sleep_time_ms);
}

