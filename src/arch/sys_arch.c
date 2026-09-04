#include "arch/sys_arch.h"
#include "debug.h"

void sys_init(void)
{
    /* lwIP calls sys_init() before anything else. The timing source is
       already initialized by Delay_Init() in main(). */
}

u32_t sys_now(void)
{
    return current_time_ms();
}
