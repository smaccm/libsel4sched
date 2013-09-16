
#include <sched/sched.h>

#include <vka/vka.h>


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

