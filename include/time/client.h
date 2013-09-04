#ifndef TIME_CLIENT_H
#define TIME_CLIENT_H

#include <time/time.h>

static inline int
time_client_split(seL4_CPtr endpoint, int parent, seL4_TimeParams params, cspacepath_t dest) 
{
    /* put the path to the cslot into the ipc buffer */
    seL4_SetCapReceivePath(dest.root, dest.capPtr, dest.capDepth);

    /* copy time params into the ipc buffer */
    time_copy_to_buffer(params, parent);

    /* make the call */
    seL4_MessageInfo_t info = seL4_MessageInfo_new(TimeSplit, 0, 0, NUM_SPLIT_ARGS);
   
    printf("Making call\n");
    seL4_Call(endpoint, info);

    /* return the child ID */
    return seL4_GetMR(0);
}

static inline int
time_client_revoke(seL4_CPtr endpoint, int parent) 
{
    /* make revoke call to manager */
    seL4_MessageInfo_t message = seL4_MessageInfo_new(TimeRevoke, 0, 0, NUM_REVOKE_ARGS);
    seL4_SetMR(0, parent);

    seL4_Call(endpoint, message);
    return seL4_GetMR(0);
}

#endif /* TIME_CLIENT_H */
