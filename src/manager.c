
#include <stdio.h>

#include <sel4/sel4.h>
#include <sel4utils/process.h>
#include <sched/manager.h>

#include <vka/vka.h>
#include <vspace/vspace.h>

seL4_CPtr  
start_time_manager(vka_t *vka, vspace_t *vspace, seL4_CPtr untyped,
        uint8_t untyped_size, uint8_t priority)
{

    sel4utils_process_t process;
    int error = sel4utils_configure_process(&process, vka, vspace, MAX_PRIO, "time-manager");
    assert(error == 0);

    /* copy the init sched_control cap into the addres space */
    cspacepath_t src;
    vka_cspace_make_path(vka, seL4_CapSchedControl, &src);
    seL4_CPtr sched_control = sel4utils_copy_cap_to_process(&process, src);
    assert(sched_control == MANAGER_SCHED_CONTROL);

    /* copy an untyped to the time manager */
    vka_cspace_make_path(vka, untyped, &src);
    seL4_CPtr untyped_cap = sel4utils_copy_cap_to_process(&process, src);
    assert(untyped_cap == MANAGER_UNTYPED);

    /* copy the root cnode of the time managers cspace to the time manager */
    vka_cspace_make_path(vka, process.cspace.cptr, &src);
    seL4_CPtr cnode = sel4utils_copy_cap_to_process(&process, src);
    assert(cnode == MANAGER_ROOT_CNODE);

    /* start: passing the size of the untyped given to the manager as an argument */
    char size[100];
    snprintf(size, 100, "%u", untyped_size);
    char *argv[1];
    argv[0] = size;
    sel4utils_spawn_process(&process, vka, vspace, 1, argv, 1);

    error = seL4_TCB_SetPriority(seL4_CapInitThreadTCB, priority);
    assert(error == seL4_NoError);

    return process.fault_endpoint.cptr;
}

