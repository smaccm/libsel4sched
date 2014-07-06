
#include <stdio.h>

#include <sel4/sel4.h>
#include <sel4utils/process.h>
#include <sched/manager.h>
#include <sched/sched.h>

#include <vka/vka.h>
#include <vspace/vspace.h>

seL4_CPtr
start_time_manager(vka_t *vka, vspace_t *vspace, seL4_CPtr cspace,
                   seL4_CapData_t cap_data, uint32_t untyped_size, uint8_t priority)
{


    /* create a sched context */
    vka_object_t sched_context = {0};
    UNUSED int error = vka_alloc_sched_context(vka, &sched_context);
    assert(error == 0);

    error = seL4_SchedControl_Configure(seL4_CapSchedControl, sched_context.cptr,
            100llu * MS_IN_S, 100llu * MS_IN_S, 100llu * MS_IN_S, 1, seL4_HardCBS, seL4_TimeTriggered);
    assert(error == 0);

    sel4utils_process_t process;
    error = sel4utils_configure_process(&process, vka, vspace, seL4_MaxPrio, sched_context.cptr,
            "time-manager");
    assert(error == 0);

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
    char *argv[2];
    argv[0] = size;
    printf("Starting time manager\n");
    error = sel4utils_spawn_process(&process, vka, vspace, 1, argv, 1);
    assert(error == 0);

    vka_object_t fault_handler_sc = sched_alloc_configure(seL4_CapSchedControl, vka, 
            sched_create_params(100llu * MS_IN_S, 100llu * MS_IN_S, 100llu * MS_IN_S, seL4_HardCBS, seL4_TimeTriggered));

    printf("Starting fault handler\n");
    sel4utils_thread_t thread;
    error = sel4utils_start_fault_handler(process.fault_endpoint.cptr, vka, vspace,
            seL4_MaxPrio, fault_handler_sc.cptr, cspace, cap_data, "time-manager", &thread);
    assert(error == 0);

    error = seL4_TCB_SetPriority(seL4_CapInitThreadTCB, priority);
    assert(error == seL4_NoError);

    return endpoint.cptr;
}

