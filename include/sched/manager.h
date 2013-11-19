#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <sel4utils/process.h>

/* this describes the cspace layout configured in the manager by start_time_manager */
enum {
    MANAGER_NULL,
    MANAGER_ROOT_CNODE = SEL4UTILS_CNODE_SLOT,
    MANAGER_FAULT_ENDPOINT = SEL4UTILS_ENDPOINT_SLOT,
    MANAGER_SCHED_CONTROL,
    MANAGER_UNTYPED,
    MANAGER_ENDPOINT,
    MANAGER_FIRST_FREE,
};



/* Start the time manager.
 *
 * @param untyped_size size of the untyped
 * @param priority priority to run the manager at
 * @param cspace the current cspace (to start the managers fault handler in)
 * @param data the current cspace data with correctly configured guard
 * @param timer_freq time frequency in khz from bootinfo
 *
 * @return a cptr in the current cspace to talk to the manager on.
 */
seL4_CPtr start_time_manager(vka_t *vka, vspace_t *vspace, seL4_CPtr cspace,
        seL4_CapData_t data, uint8_t untyped_size, uint8_t priority, uint32_t timer_freq);

#endif /* TIME_MANAGER_H */
