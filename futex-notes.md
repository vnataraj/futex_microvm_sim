##Futex in MicroVM

###Requirements for user-level locking

Must allow implemention of:

* **Fair** locking: granted in order of request

    Leads to **convoy problem** (running at speed of slowest process)  

* **Random** locking: when released, wake all waiting processes to recontend for the lock  

    Solves convoy problem but leads to **thundering herd**

* **Greedy** locking: when released, give releasing task chance to reacquire before waking others  
 
    Can lead to starvation if lock owning process keeps relocking

* **Spin** locking

Thus, 3 choices for a contending application:

1. **Block**, waiting to be notified;

2. **Yield**, waiting until resched to attempt to acquire the lock; and

3. **Spin**, eating CPU cycles until lock is released.

__Goals__: Avoid system calls and context switching

Contended: block (system call)

* Requires **kernel object** with **waiting queues**

* Before blocking, task may attempt to spin on user lock (occasionally yielding processor)

Uncontended: user-level locking mechanism in shared memory

* May be in different virtual addresses in different address spaces

* Requires atomic operations in user space

__Global lock identity__: tuple of memory object + offset **(B, O)**; three main types for B:

1. **Anonymous** memory: only usable between threads of same process

2. **Shared** memory: not available on Java natively

3. **Memory-mapped files**: java.nio

Race conditions still possible (sequence of lock manipulation and system calls not atomic); must be resolved in kernel

Particular interest: mutex, semaphores, rw locks

Recovery of "dead" locks problematic

###Linux Fast User-level Locking

Basic ulock_t structure is as follows:

    typedef struct ulock_t {
        long status;
    } ulock_t;

Basic approach above allows implementing user-level semaphores that only call the kernel during contention

* However, possible race condition: lock can receive wakeup call before waiting process puts itself into kernel wait queue

**Ulocks** require a ulock_t consisting of a **lock word** and the **number of kernel wait queues required** (2 for multi-reader locks, 1 otherwise)

* Semaphores and mutexes handled as above, **but** kernel internal wait queues use kernel internal semaphores initialized to 0 - no race condition due to premature wakeup call

* rw-locks require 3 fields - write locked (1 bit), writes waiting (15 bits), readers (16 bits)

    Require atomic CAS

