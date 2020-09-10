#include <pthread.h>
#include <time.h>
#include "bsp.h"

void bsp_sleep(unsigned int sleep_time_ms)
{
    time_t seconds = sleep_time_ms / 1000;
    long remainder_nanoseconds = (sleep_time_ms % 1000) * 1000000;
    struct timespec time_to_sleep = { seconds, remainder_nanoseconds };
    (void)nanosleep(&time_to_sleep, NULL);
}

