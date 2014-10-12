/*
 * Fast Userspace Read/Write locks, built on top of mutexes.
 *
 * The design was done by Paul Mackerras.  Rusty did the typing and naming.
 */
#ifndef	_FURWOCK_H
#define	_FURWOCK_H
#include "usersem.h"

/* You should never touch this structure directly! */
struct furwock
{
	/* Protects the data. */
	struct futex gate;
	/* If writer waiting, gate held and counter = # readers - 1.
	   Otherwise, counter = # readers. */
	int count;
	/* Simple downed semaphore for writer to sleep on. */
	struct futex wait;
};

extern void furwock_init(struct furwock *rwock);

static inline int furwock_read_down(struct furwock *rwock)
{
	int ret;

	ret = futex_down(&rwock->gate);
	if (ret == 0) {
		__atomic_inc(&rwock->count);
		futex_up(&rwock->gate);
	}
	return ret;
}

static inline int furwock_read_trydown(struct furwock *rwock)
{
	int ret;

	ret = futex_trydown(&rwock->gate);
	if (ret == 0) {
		if(rwock->count >= 0)
			__atomic_inc(&rwock->count);
		futex_up(&rwock->gate);
	}
	return ret;
}


static inline void furwock_read_up(struct furwock *rwock)
{
	/* Last one out wakes and writer waiting. */
	if (__furwock_dec_negative(&rwock->count))
		futex_up(&rwock->wait);
}

static inline int furwock_write_down(struct furwock *rwock)
{
	int ret;

	ret = futex_down(&rwock->gate);
	if (ret == 0) {
		if (__furwock_dec_negative(&rwock->count))
			return ret;
		futex_down(&rwock->wait);
	}
	return ret;
}

static inline int furwock_write_trydown(struct furwock *rwock)
{
	int ret;

	ret = futex_trydown(&rwock->gate);
	if (ret == 0) {
		if (__furwock_dec_negative(&rwock->count))
			return ret;
		futex_down(&rwock->wait);
	}
	return ret;
}

static inline void furwock_write_up(struct furwock *rwock)
{
	rwock->count = 0;
	__futex_commit();
	futex_up(&rwock->gate);
}

#endif /* _FURWOCK_H */
