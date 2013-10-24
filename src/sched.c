
#include <sched/sched.h>
#include <math.h>

#include <vka/vka.h>

static double sched_period;

#define FS_IN_US 1000000000.0f

void 
sched_set_timer_period(uint32_t period) 
{
    sched_period = period / FS_IN_US;
    printf("Sched period set to %lf\n", sched_period); 
}

int
sched_configure(seL4_SchedControl sched_control, seL4_SchedContext sched_context,
                seL4_SchedParams params, bool bindable)
{

    printf("params: us\n");
    sched_print_params(params);
    /* convert params from ms to ticks */
    params.period = (uint64_t) roundl((long double) params.period / sched_period);
    params.relativeDeadline = (uint64_t) roundl((long double) params.relativeDeadline / sched_period);
    params.execution = (uint64_t) roundl((long double) params.execution / sched_period);
    printf("params: ticks\n");
    sched_print_params(params);

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
    assert(error == 0);

    return sc;
}

