#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include "usersem.h"

#ifndef TIMEVAL_TO_TIMESPEC
# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
#endif

typedef struct futex nonpthread_mutex_t;

typedef struct 
{
	int counter;
} nonpthread_cond_t;

typedef struct
{
	struct futex mutex;
	int num_left, acks;
	int initial_count;
} nonpthread_barrier_t;

#define NONPTHREAD_MUTEX_INITIALIZER FUTEX_INITIALIZER
#define NONPTHREAD_COND_INITIALIZER { 0 }

int nonpthread_barrier_init(nonpthread_barrier_t *barrier,
			    void *addr,
			    unsigned int count)
{
	futex_init(&barrier->mutex);
	barrier->num_left = barrier->initial_count = count;
	barrier->acks = 0;
	return 0;
}

static inline void wait_for(int *addr, int value)
{
	int v;

	while ((v = *addr) != value)
		sys_futex(addr, FUTEX_WAIT, v, NULL);
}

int nonpthread_barrier_wait(nonpthread_barrier_t *barrier)
{
	while (futex_down(&barrier->mutex) < 0) {
		if (errno != EINTR)
			return -1;
	}

	if (--barrier->num_left == 0) {
		/* Make sure value is nailed and wake others */
		__futex_commit();
		sys_futex(&barrier->num_left, FUTEX_WAKE,
			  barrier->initial_count-1, NULL);

		/* Wait for them all to ack. */
		wait_for(&barrier->acks, barrier->initial_count - 1);

		/* Reset barrier */
		barrier->num_left = barrier->initial_count;
		barrier->acks = 0;
		futex_up(&barrier->mutex);
		return 0; /* NONPTHREAD_BARRIER_SERIAL_THREAD */
	} else {
		futex_up(&barrier->mutex);
		/* Wait for decrement to zero. */
		wait_for(&barrier->num_left, 0);

		/* Ack: last ack wakes thread. */
		__atomic_inc(&barrier->acks);
		if (barrier->acks == barrier->initial_count - 1)
			sys_futex(&barrier->acks, FUTEX_WAKE, 1, NULL);
		return 1;
	}
}

int nonpthread_cond_signal(nonpthread_cond_t *cond)
{
	/* Unreliable if they don't hold mutex, but that's as per spec */
	cond->counter++;
	return sys_futex(&cond->counter, FUTEX_WAKE, 1, NULL);
}

int nonpthread_cond_broadcast(nonpthread_cond_t *cond)
{
	/* Unreliable if they don't hold mutex, but that's as per spec */
	cond->counter++;
	return sys_futex(&cond->counter, FUTEX_WAKE, INT_MAX, NULL);
}

int nonpthread_cond_wait(nonpthread_cond_t *cond, nonpthread_mutex_t *mutex)
{
	int value, ret, saved_errno;

	value = cond->counter;
	futex_up(mutex);

	ret = sys_futex(&cond->counter, FUTEX_WAIT, value, NULL);

	saved_errno = errno;
	while (futex_down(mutex) < 0 && errno == EINTR);
	errno = saved_errno;

	return ret;
}

int nonpthread_cond_timedwait(nonpthread_cond_t *cond,
			   nonpthread_mutex_t *mutex,
			   const struct timespec *abstime)
{
	struct timeval _now;
	struct timespec now, rel;
	int value, ret, saved_errno;

	value = cond->counter;
	futex_up(mutex);

 again:
	/* Absolute to relative */
	gettimeofday(&_now, NULL);
	TIMEVAL_TO_TIMESPEC(&_now, &now);
	if (now.tv_sec > abstime->tv_sec
	    || (now.tv_sec == abstime->tv_sec
		&& now.tv_nsec > abstime->tv_nsec))
		return ETIMEDOUT;

	rel.tv_sec = now.tv_sec - abstime->tv_sec;
	rel.tv_nsec = now.tv_nsec - abstime->tv_nsec;
	if (rel.tv_nsec < 0) {
		--rel.tv_sec;
		rel.tv_nsec += 1000000000;
	}

	ret = sys_futex(&cond->counter, FUTEX_WAIT, value, &rel);
	if (ret < 0 && errno == EINTR)
		goto again;

	saved_errno = errno;
	while (futex_down(mutex) < 0 && errno == EINTR);
	errno = saved_errno;
	return ret;
}
