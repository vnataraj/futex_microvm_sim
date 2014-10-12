int
futex_wait(struct futex * f, int val, struct timespec * reltime) {
    ret = 0;
    Mark process as INTERRUPTIBLE

    Hash ("struct page *" + offset) -> index [i] into hash table

    chain "struct futex_q" to tail of hashtable[i]
        typedef struct futex_q {
            process *     prptr;
            struct page * p;
            offset        o;
        } futex_q;

    Map into low memory (kernel) if in high memory

    Read futex value -> valread

    Unmap (if mapped before)

    if ( valread != val )
        ret = -EWOULDBLOCK;
    else {
        if ( reltime != NULL )  sleep(reltime);
        else                    sleep(INDEFINITELY);

        if ( timeout )
            ret = ETIMEDOUT;
        else
            ret = EINTR;
    }

    Try to remove our "struct futex_q" from the hash table

    if ( already removed )
        return 0;
    else
        return ret;
}

int
futex_wake(struct futex * f, int val, struct timespec * reltime) {
    
    for ( int i = 0; i < val; i++ ) {
        attempt to wake next process in appropriate hash bucket
    }

    return number of processes actually woken;
}
