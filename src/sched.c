
#include <sched/sched.h>
#include <math.h>

#include <vka/vka.h>

static uint32_t ticks_per_ms;

void 
sched_set_timer_khz(uint32_t freq) 
{

    ticks_per_ms = period;
    printf("Ticks_per_ms = %u\n", ticks_per_ms);
    assert(ticks_per_ms > 0);
}

int
sched_configure(seL4_SchedControl sched_control, seL4_SchedContext sched_context,
                seL4_SchedParams params, bool bindable)
{
    /* TODO this may not be neccessary as we can multiply in kernel. */
    params.period = (uint64_t) params.period * ticks_per_ms;
    params.relativeDeadline = params.relativeDeadline * ticks_per_ms;
    params.execution = params.execution * ticks_per_ms;

    if (params.period == 0) {
        LOG_ERROR("Period must be > 0\n");
        return seL4_InvalidArgument;
    }

    /* leave params as ms */
    return seL4_SchedControl_Configure(sched_control, sched_context, params.period ,
        params.relativeDeadline, params.execution,
        params.relativeDeadline / params.period, params.cbs, bindable);
}

vka_object_t
sched_alloc_configure(seL4_SchedControl sched_control, vka_t *vka, seL4_SchedParams params,
        bool bindable) 
{
    vka_object_t sc = {0};

    int error = vka_alloc_sched_context(vka, &sc);
    assert(error == 0);

    error = sched_configure(sched_control, sc.cptr, params, bindable);
    LOG_ERROR("Sched_configure failed with error %d\n", error);
    assert(error == 0);

    return sc;
}

