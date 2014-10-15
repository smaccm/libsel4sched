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

#define NUM_SPLIT_ARGS 9
#define NUM_REVOKE_ARGS 1
#define ODIN 1

#ifdef CONFIG_USER_DEBUG_BUILD
static inline void sched_print_params(seL4_SchedParams_t params)
{
    LOG_INFO("p: %llu\t", params.period);
    LOG_INFO("d: %llu\t", params.deadline);
    LOG_INFO("e: %llu\t", params.budget);
    LOG_INFO("cbs: %s\n", seL4_SchedFlags_get_cbs(params.flags) == seL4_HardCBS ? "Hard" : "Soft");
    LOG_INFO("trigger: %s\n", seL4_SchedFlags_get_trigger(params.flags) == seL4_TimeTriggered ? 
            "Time triggered" : "Event triggered");
    LOG_INFO("data: %x\n", seL4_SchedFlags_get_data(params.flags));
    
}
#else
#define sched_print_params(x)
#endif

/* define the timer frequency in khz. This value is taken from bootinfo */
void sched_set_timer_khz(uint32_t freq);

static inline void
sched_copy_to_buffer(seL4_SchedParams_t params, int id)
{
    seL4_SetMR(0, id);
    seL4_SetMR(1, (uint32_t) (params.period >> 32));
    seL4_SetMR(2, (uint32_t) params.period);
    seL4_SetMR(3, (uint32_t) (params.budget >> 32));
    seL4_SetMR(4, (uint32_t) params.budget);
    seL4_SetMR(5, (uint32_t) (params.deadline >> 32));
    seL4_SetMR(6, (uint32_t) params.deadline);
    seL4_SetMR(7, params.flags.words[0]);
}

static inline void
sched_copy_from_buffer(seL4_SchedParams_t *params, int *id)
{
    *id = seL4_GetMR(0);
    params->period = ((uint64_t) seL4_GetMR(1) << 32) + seL4_GetMR(2);
    params->budget = ((uint64_t) seL4_GetMR(3) << 32) + seL4_GetMR(4);
    params->deadline = ((uint64_t) seL4_GetMR(5) << 32) + seL4_GetMR(6);
    params->flags.words[0] = seL4_GetMR(7);
}


static inline seL4_SchedParams_t
sched_create_params(uint64_t period, uint64_t deadline, uint64_t budget, seL4_CBS cbs,
        seL4_TaskType trigger)
{
    /* this is currently what we support */
    assert(deadline == period);

    return (seL4_SchedParams_t) {
        .period = period,
        .deadline = deadline,
        .budget = budget,
        .flags = seL4_SchedFlags_new(trigger, cbs, 0)
    };
}

static inline seL4_SchedParams_t
sched_create_params_with_data(uint64_t period, uint64_t deadline, uint64_t budget, seL4_CBS cbs,
        seL4_TaskType trigger, uint32_t data)
{
    /* this is currently what we support */
    assert(deadline == period);

    return (seL4_SchedParams_t) {
        .period = period,
        .deadline = deadline,
        .budget = budget,
        .flags = seL4_SchedFlags_new(trigger, cbs, data)
    };
}

static inline seL4_SchedParams_t
timeslice_params(uint64_t timeslice_us)
{
    return sched_create_params(timeslice_us, timeslice_us, timeslice_us, seL4_HardCBS,
                               seL4_TimeTriggered);
}


vka_object_t sched_alloc_configure(seL4_SchedControl sched_control, vka_t  *vka, seL4_SchedParams_t params);

/* Call seL4_SchedControl_Configure */
int sched_configure(seL4_SchedControl sched_control, seL4_SchedContext sched_context,
                    seL4_SchedParams_t params);

#endif /* SCHED_H */
