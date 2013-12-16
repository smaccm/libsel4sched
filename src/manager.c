
#include <stdio.h>

#include <sel4/sel4.h>
#include <sel4utils/process.h>
#include <sched/manager.h>
#include <sched/sched.h>

#include <vka/vka.h>
#include <vspace/vspace.h>

seL4_CPtr
start_time_manager(vka_t *vka, vspace_t *vspace, seL4_CPtr cspace,
                   seL4_CapData_t cap_data, uint8_t untyped_size, uint8_t priority, uint32_t timer_freq)
{

    sel4utils_process_t process;
    UNUSED int error = sel4utils_configure_process(&process, vka, vspace, MAX_PRIO, "time-manager");
    assert(error == 0);

    sched_set_timer_khz(timer_freq);

    /* copy the init sched_control cap into the addres space */
    cspacepath_t src;
    vka_cspace_make_path(vka, seL4_CapSchedControl, &src);
    UNUSED seL4_CPtr sched_control = sel4utils_copy_cap_to_process(&process, src);
    assert(sched_control == MANAGER_SCHED_CONTROL);

    /* copy an untyped to the time manager */
    /* create an untyped of the right size and copy to manager */
    vka_object_t untyped = {0};
    error = vka_alloc_untyped(vka, untyped_size, &untyped);
    assert(error == 0);

    vka_cspace_make_path(vka, untyped.cptr, &src);
    UNUSED seL4_CPtr untyped_cap = sel4utils_copy_cap_to_process(&process, src);
    assert(untyped_cap == MANAGER_UNTYPED);

    /* copy an enpoint to the time manager */
    vka_object_t endpoint = {0};
    error = vka_alloc_endpoint(vka, &endpoint);
    assert(error == 0);
    vka_cspace_make_path(vka, endpoint.cptr, &src);
    
    UNUSED seL4_CPtr endpoint_cap = sel4utils_copy_cap_to_process(&process, src);
    assert(endpoint_cap == MANAGER_ENDPOINT);

    /* start: passing the size of the untyped given to the manager as an argument */
    char size[100];
    snprintf(size, 100, "%u", untyped_size);
    char timer_period[100];
    snprintf(timer_period, 100, "%u", timer_freq);
    char *argv[2];
    argv[0] = size;
    argv[1] = timer_period;
    error = sel4utils_spawn_process(&process, vka, vspace, 2, argv, 1);
    assert(error == 0);

    sel4utils_thread_t thread;
    error = sel4utils_start_fault_handler(process.fault_endpoint.cptr, vka, vspace,
            MAX_PRIO, cspace, cap_data, "time-manager", &thread);
    assert(error == 0);

    error = seL4_TCB_SetPriority(seL4_CapInitThreadTCB, priority);
    assert(error == seL4_NoError);

    return endpoint.cptr;
}

