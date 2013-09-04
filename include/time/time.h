#ifndef TIME_SHARED_H
#define TIME_SHARED_H

#include <autoconf.h>
#include <string.h>

#include <sel4utils/util.h>

typedef enum {
    TimeSplit,
    TimeRevoke
} time_op_t;

typedef struct time {
    int id;
    seL4_CPtr cptr;
} time_t;

#define NUM_SPLIT_ARGS 8
#define NUM_REVOKE_ARGS 1
#define ODIN 1

#ifdef CONFIG_DEBUG_BUILD
static inline void time_print_params(seL4_TimeParams params) 
{
    LOG_INFO("p: %llu\t", params.period);
    LOG_INFO("e: %llu\t", params.relativeDeadline);
    LOG_INFO("d: %llu\t", params.execution);
    LOG_INFO("cbs: %s\n", params.cbs == seL4_HardCBS ? "Hard" : "Soft");
}
#else
#define time_print_params(x) 
#endif 

static inline void 
time_copy_to_buffer(seL4_TimeParams params, int parent) 
{
    seL4_SetMR(0, parent);
    seL4_SetMR(1, (uint32_t) (params.period >> 32));
    seL4_SetMR(2, (uint32_t) params.period);
    seL4_SetMR(3, (uint32_t) (params.execution >> 32));
    seL4_SetMR(4, (uint32_t) params.execution);
    seL4_SetMR(5, (uint32_t) (params.relativeDeadline >> 32));
    seL4_SetMR(6, (uint32_t) params.relativeDeadline);
    seL4_SetMR(7, params.cbs);
}

static inline void 
time_copy_from_buffer(seL4_TimeParams *params, int *parent)
{
    *parent = seL4_GetMR(0);
    params->period = ((uint64_t) seL4_GetMR(1) << 32) + seL4_GetMR(2);
    params->execution = ((uint64_t) seL4_GetMR(1) << 32) + seL4_GetMR(2);
    params->relativeDeadline = ((uint64_t) seL4_GetMR(1) << 32) + seL4_GetMR(2);
    params->cbs = seL4_GetMR(7);
}



#endif /* TIME_H */
