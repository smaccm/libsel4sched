#ifndef SCHED_CLIENT_H
#define SCHED_CLIENT_H

#include <stdio.h>
#include <stddef.h>

#include <vka/cspacepath_t.h>

#include <sel4/sel4.h>

#include <sched/sched.h>

#include <vka/vka.h>




/**
 * IPC the sched manager to request a split. 
 *
 * @param endpoint the endpoint to talk to the manager on
 * @param parent the id of the parent reservation to split
 * @param params the params of the reservation to attempt to split
 * @param dest path to the cslot to the put the new cap into
 *
 * @return the id of the new sched object. -1 on failure.
 */
int sched_client_split(seL4_CPtr endpoint, int parent, seL4_SchedParams params, cspacepath_t dest); 

/**
 * As above, but perform the allocations for the user.
 * This function assumes that malloc works.
 *
 * @param vka interface to allocate cslot with
 * @param endpoint the endpoint to talk to the manager
 * @param parent the id of the parent reservation to split
 * @param params the params of the reservation to attempt to split
 *
 * @return NULL on failure, an initialised sched object otherwise (with cslot and id). 
 */
sched_t *sched_client_alloc_split(vka_t *vka, seL4_CPtr endpoint, 
        int32_t parent, seL4_SchedParams params);

/**
 * IPC the sched manager to revoke the children of a reservation.
 *
 * 
 * @param endpoint the endpoint to talk to the manager
 * @param parent the id of the parent reservation to revoke
 * @return 0 on success
 */
int sched_client_revoke(seL4_CPtr endpoint, int parent);

/** 
 * Free the objects allocaed by alloc_and_split_sched. 
 * Does not revoke (call this separately on the parent).
 *
 * @param vka interface to that the sched reservations cslot was allocated with.
 * @param sched the allocated sched object to free. 
 */
void sched_client_free(vka_t *vka, sched_t *sched);
#endif /* sched_CLIENT_H */
