#include <windows.h>
#include <time.h>
#include "bsp.h"

void bsp_sleep(unsigned int sleep_time_ms)
{
    Sleep(sleep_time_ms);
}

