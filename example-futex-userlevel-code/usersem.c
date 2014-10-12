/* Fast userspace semaphores. 
 * Based on work by Matthew Kirkwood <matthew@hairy.beasts.org>. */
#include "usersem.h"
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
/* This must include the futex call */
#include <asm/unistd.h>

#ifndef PROT_SEM
#define PROT_SEM		0x08
#endif

#define __NR_sys_futex __NR_futex

_syscall4(int,sys_futex,
	  int *, futex,
	  int, op,
	  int, val,
	  struct timespec *, rel);

int futex_region(void *area, size_t size)
{
	int pagesize = sysconf(_SC_PAGE_SIZE);

	/* Align pointer to previous start of page */
	area = (void *)((unsigned long)area / pagesize * pagesize);

	/* Round size up to page size */
	size = (size + pagesize - 1) / pagesize * pagesize;

	return mprotect(area, size, PROT_READ|PROT_WRITE|PROT_SEM);
}

/* Returns -1 on fail, 0 on wakeup, 1 on pass, 2 on didn't sleep */
int __futex_down_slow(struct futex *futx, int val, struct timespec *rel)
{
	if (sys_futex(&futx->count, FUTEX_WAIT, val, rel) == 0) {
		/* <= in case someone else decremented it */
		if (futx->count <= FUTEX_PASSED) {
			futx->count = -1;
			return 1;
		}
		return 0;
	}
	/* EWOULDBLOCK just means value changed before we slept: loop */
	if (errno == EWOULDBLOCK)
		return 2;
	return -1;
}

int __futex_up_slow(struct futex *futx)
{
	futx->count = 1;
	__futex_commit();
	return sys_futex(&futx->count, FUTEX_WAKE, 1, NULL);
}

int futex_await(struct futex *futx, int signal)
{
	return sys_futex(&futx->count, FUTEX_FD, signal, NULL);
}

void futex_init(struct futex *futx)
{
	futx->count = 1;
	__futex_commit();
}
