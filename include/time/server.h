#ifndef TIME_SERVER_H
#define TIME_SERVER_H

#include <time/time.h>

static inline int 
time_server_configure(seL4_TimeControl time_control, seL4_Time time, seL4_TimeParams params) 
{
    return seL4_TimeControl_Configure(time_control, time, params.period,
                  params.relativeDeadline, params.execution, params.cbs);
}

#endif /* TIME_SERVER_H */
