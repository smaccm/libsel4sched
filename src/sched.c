
#include <sched/sched.h>
#include <math.h>

#include <vka/vka.h>

int
sched_configure_khz(seL4_SchedControl sched_control, seL4_SchedContext sched_context,
                seL4_SchedParams params, bool bindable, uint32_t khz) {
 
    params.period = (uint64_t) params.period * khz;
    params.relativeDeadline = params.relativeDeadline * khz;
    params.execution = params.execution * khz;

    return sched_configure(sched_control, sched_context, params, bindable);
}


int
sched_configure(seL4_SchedControl sched_control, seL4_SchedContext sched_context,
                seL4_SchedParams params, bool bindable)
{
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
    
    if (error != 0) {
        LOG_ERROR("Sched_configure failed with error %d\n", error);
        assert(error == 0);
    }

    return sc;
}

