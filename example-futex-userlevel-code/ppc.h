/* (C) Rusty Russell <rusty@rustcorp.com.au> */
/* Stolen from Linux kernel (Paul Mackerras) */

/* Atomic dec: return new value. */
static __inline__ int __futex_down(int *counter)
{
	int val;

	/* Don't decrement if already negative. */
	if (*counter < 0)
		return *counter;

	/* The isync here stops the chip from pulling loads inside the
           lock above this code */
	__asm__ __volatile__(
"1:	lwarx	%0,0,%1		# __down\n\
	addic	%0,%0,-1\n\
	stwcx.	%0,0,%1\n\
	bne-	1b\n\
	isync"
	: "=&r" (val)
	: "r" (counter)
	: "cc", "memory");

	return val;
}

/* Atomic inc: return 1 if counter incremented from 0 to 1. */
static __inline__ int __futex_up(int *counter)
{
	int val;

	/* sync makes sure reads AND writes are committed before we
           unlock. */
	__asm__ __volatile__(
"	sync\n\
1:	lwarx	%0,0,%1		# __up\n\
	addic	%0,%0,1\n\
	stwcx.	%0,0,%1\n\
	bne-	1b"
	: "=&r" (val)
	: "r" (counter)
	: "cc", "memory");

	return (val == 1);
}

/* Simple atomic inc. */
static __inline__ void __atomic_inc(int *counter)
{
	int val;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%2		# __furwock_inc\n\
	addic	%0,%0,1\n"
"	stwcx.	%0,0,%2 \n\
	bne-	1b"
	: "=&r" (val), "=m" (*counter)
	: "r" (counter), "m" (*counter)
	: "cc");
}

/* Atomic dec: return 1 if result is negative */
static __inline__ int __furwock_dec_negative(int *counter)
{
	int val;

	__asm__ __volatile__(
"1:	lwarx	%0,0,%1		# __furwock_dec_negative\n\
	addic	%0,%0,-1\n"
"	stwcx.	%0,0,%1\n\
	bne-	1b"
	"\n\tisync"
	: "=&r" (val)
	: "r" (counter)
	: "cc", "memory");

	return val < 0;
}

/* Commit the write, so it happens before we send the semaphore to
   anyone else */
static __inline__ void __futex_commit(void)
{
	__asm__ __volatile__ ("sync" : : : "memory");
}
