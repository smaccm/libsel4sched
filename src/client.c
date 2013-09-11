#ifndef SCHED_CLIENT_H
#define SCHED_CLIENT_H

#include <sel4/sel4.h>
#include <stdio.h>
#include <stdlib.h>

#include <sched/sched.h>
#include <sched/client.h>

#include <vka/vka.h>

int
sched_client_split(seL4_CPtr endpoint, int parent, seL4_SchedParams params, cspacepath_t dest) 
{
    /* put the path to the cslot into the ipc buffer */
    seL4_SetCapReceivePath(dest.root, dest.capPtr, dest.capDepth);

    /* copy sched params into the ipc buffer */
    sched_copy_to_buffer(params, parent);

    /* make the call */
    seL4_MessageInfo_t info = seL4_MessageInfo_new(Split, 0, 0, NUM_SPLIT_ARGS);
   
    seL4_Call(endpoint, info);

    /* return the child ID */
    return seL4_GetMR(0);
}

sched_t *
sched_client_alloc_split(vka_t *vka, seL4_CPtr endpoint, int32_t parent, seL4_SchedParams params)
{
    sched_t *sched = malloc(sizeof (sched_t));
    if (sched == NULL) {
        return NULL;
    }

    int error = vka_cspace_alloc(vka, &sched->cptr);
    if (error != 0) {
        free(sched);
        return NULL;
    }

    assert(sched->cptr != 0);

    cspacepath_t path;
    vka_cspace_make_path(vka, sched->cptr, &path);
    
    sched->id = sched_client_split(endpoint , parent, params, path);
    
    if (sched->id == -1) {
        vka_cspace_free(vka, sched->cptr);
        free(sched);
        return NULL;
    } 

    return sched;
}
    
void
sched_client_revoke(seL4_CPtr endpoint, int parent) 
{
    /* make revoke call to manager */
    seL4_MessageInfo_t message = seL4_MessageInfo_new(Revoke, 0, 0, NUM_REVOKE_ARGS);
    seL4_SetMR(0, parent);

    seL4_Call(endpoint, message);
    int result = seL4_GetMR(0);
    assert(result == 0);
}

void
sched_client_free(vka_t *vka, sched_t *sched)
{
    vka_cspace_free(vka, sched->cptr);
    free(sched);
}



#endif /* SCHED_CLIENT_H */