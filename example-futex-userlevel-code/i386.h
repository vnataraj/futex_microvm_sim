/* (C) Matthew Kirkwood <matthew@hairy.beasts.org>
   (C) 2002 Rusty Russell IBM <rusty@rustcorp.com.au>
 */

/* Atomic dec: return new value. */
static __inline__ int __futex_down(int *counter)
{
	int val;
	unsigned char eqz;

	/* Don't decrement if already negative. */
	val = *counter;
	if (val < 0)
		return val;

	/* Damn 386: no cmpxchg... */
	__asm__ __volatile__(
		"lock; decl %0; sete %1"
		:"=m" (*counter), "=qm" (eqz)
		:"m" (*counter) : "memory");

	/* We know if it's zero... */
	if (eqz) return 0;
	/* Otherwise, we have no way of knowing value.  Guess -1 (if
           we're wrong we'll spin). */
	return -1;
}

/* Atomic inc: return 1 if counter incremented from 0 to 1. */
static __inline__ int __futex_up(int *c)
{
	int r = 1;

	/* This actually tests if result >= 1.  Damn 386. --RR */
	__asm__ __volatile__ (
	"	lock; incl %1\n"
	"	jg 1f\n"
	"	decl %0\n"
	"1:\n"
		: "=q"(r), "=m"(*c) : "0"(r)
	);
	return r;
}

/* Simple atomic increment. */
static __inline__ void __atomic_inc(int *c)
{
	__asm__ __volatile__(
		"lock; incl %0"
		:"=m" (*c)
		:"m" (*c));
}

/* Atomic decrement, and return 1 if result is negative. */
static __inline__ int __furwock_dec_negative(int *c)
{
	unsigned char r;

	__asm__ __volatile__(
		"lock; decl %0; setl %1"
		:"=m" (*c), "=qm" (r)
		:"m" (*c) : "memory");

	return r;
}

/* Commit the write, so it happens before we send the semaphore to
   anyone else */
static __inline__ void __futex_commit(void)
{
	/* Probably overkill, but some non-Intel clones support
           out-of-order stores, according to 2.5.5-pre1's
           linux/include/asm-i386/system.h */
	__asm__ __volatile__ ("lock; addl $0,0(%%esp)": : :"memory");
}
