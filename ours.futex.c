int
futex_wait(struct futex * f, int val, struct timespec * reltime) {
    ret = 0;
    
    // Mark process as INTERRUPTIBLE
    //      Likely unnecessary because our futex implementation does not enter kernel space

    // Hash ("struct page *" + offset) -> index [i] into hash table
    //      Instead of a page pointer it will be a memory mapped file
    Open memory-mapped file -> address of first byte in fmm
    Hash ( fmm + offset ) -> index [i] into hash table

    chain "struct futex_q" to tail of hashtable[i]
        typedef struct futex_q {
            process *     prptr;
            char *        fmm;
            offset        o;
        } futex_q;

    // Map into low memory (kernel) if in high memory
    //      Unnecessary - we aren't in kernel space and this isn't a syscall

    Read futex value -> valread

    // Unmap (if mapped before)
    //      Unnecessary - we aren't in kernel space and this isn't a syscall

    if ( valread != val )
        ret = -EWOULDBLOCK;
    else {
        // if ( reltime != NULL )   sleep(reltime);
        // else                     sleep(INDEFINITELY);
        //      No sleep() for the wicked.  Sleep is for people who are allowed to use syscalls
        if ( reltime != NULL )
            spin(reltime);
        else
            return 0;   // if timeout undefined, just add to the wait queue
        // SOME_PREDEFINED_TIME - possibly 60s or so?

        if ( timeout )
            ret = ETIMEDOUT;
        // else
        //    ret = EINTR;
        //      We can't do anything about interrupts, so whatever
    }

    Try to remove our "struct futex_q" from the hash table

    if ( already removed )
        return 0;
    else
        return ret;
}

int
futex_wake(struct futex* f, int val, struct timespec* reltime){
  for( int i = 0 ; i < val; i++){
    find hash bucket and wake next process in wait queue
    if( process awoken)   count++;
    else continue;
  }
  return number of processes woken
 }
