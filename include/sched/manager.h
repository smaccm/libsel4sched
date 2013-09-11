#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <sel4utils/process.h>

/* this describes the cspace layout configured in the manager by start_time_manager */
enum {
    MANAGER_NULL,
    MANAGER_ENDPOINT = SEL4UTILS_ENDPOINT_SLOT,
    MANAGER_SCHED_CONTROL,
    MANAGER_UNTYPED,
    MANAGER_ROOT_CNODE,
    MANAGER_FIRST_FREE
};



/* Start the time manager.
 * 
 * @param untyped untyped to copy to manager for allocations
 * @param untyped_size size of the untyped
 * @param priority priority to run the manager at
 *
 * @return a cptr in the current cspace to talk to the manager on.
 */
seL4_CPtr start_time_manager(vka_t *vka, vspace_t *vspace, seL4_CPtr untyped, 
        uint8_t untyped_size, uint8_t priority);

#endif /* TIME_MANAGER_H */
