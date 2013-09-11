#ifndef SCHED_SHARED_H
#define SCHED_SHARED_H

#include <autoconf.h>
#include <string.h>

#include <sel4utils/util.h>

#include <vka/vka.h>
#include <vka/object.h>

typedef enum {
    Split,
    Revoke
} sched_op_t;

typedef struct sched {
    int id;
    seL4_CPtr cptr;
} sched_t;

#define NUM_SPLIT_ARGS 8
#define NUM_REVOKE_ARGS 1
#define ODIN 1

#ifdef CONFIG_DEBUG_BUILD
static inline void sched_print_params(seL4_SchedParams params) 
{
    LOG_INFO("p: %llu\t", params.period);
    LOG_INFO("d: %llu\t", params.relativeDeadline);
    LOG_INFO("e: %llu\t", params.execution);
    LOG_INFO("cbs: %s\n", params.cbs == seL4_HardCBS ? "Hard" : "Soft");
}
#else
#define sched_print_params(x) 
#endif 

static inline void 
sched_copy_to_buffer(seL4_SchedParams params, int parent) 
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
sched_copy_from_buffer(seL4_SchedParams *params, int *parent)
{
    *parent = seL4_GetMR(0);
    params->period = ((uint64_t) seL4_GetMR(1) << 32) + seL4_GetMR(2);
    params->execution = ((uint64_t) seL4_GetMR(3) << 32) + seL4_GetMR(4);
    params->relativeDeadline = ((uint64_t) seL4_GetMR(5) << 32) + seL4_GetMR(6);
    params->cbs = seL4_GetMR(7);
}


static inline seL4_SchedParams
sched_create_params(uint64_t period, uint64_t relative_deadline, uint64_t execution, seL4_CBS cbs) 
{
    return (seL4_SchedParams) {
        .period = period,
        .relativeDeadline = relative_deadline,
        .execution = execution,
        .cbs = cbs
    };
}

static inline int 
sched_configure(seL4_SchedControl sched_control, seL4_SchedContext sched_context, seL4_SchedParams params) 
{
    return seL4_SchedControl_Configure(sched_control, sched_context, params.period,
                  params.relativeDeadline, params.execution, params.cbs);
}


vka_object_t sched_alloc_configure(seL4_SchedControl sched_control, vka_t  *vka, seL4_SchedParams params);

#endif /* SCHED_H */
