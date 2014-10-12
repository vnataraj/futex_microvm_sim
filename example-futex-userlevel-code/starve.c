#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <sched.h>
#include "usersem.h"

#ifndef PROT_SEM
#define PROT_SEM 0x8
#endif

static void spinner(struct futex *sem, int fair)
{
	while (1) {
		futex_down(sem);
		sleep(1);
		if (fair)
			futex_up_fair(sem);
		else
			futex_up(sem);
	}
}

/* Test maximum time to lock given furious spinners. */
int main(int argc, char *argv[])
{
	struct futex *sem;
	unsigned int i;
	unsigned long maxtime = 0;
	int fd, fair = 0;
	pid_t children[100];

	if (argc != 4 && argc != 5) {
		fprintf(stderr, "Usage: starve [--fair] <file> <numspinners> <iterations>\n");
		exit(1);
	}

	if (strcmp(argv[1], "--fair") == 0) {
		fair = 1;
		argv++;
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("opening file");
		exit(1);
	}
	sem = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (sem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	if (futex_region(sem, 4096) != 0) {
		perror("futex_region");
		exit(1);
	}

	futex_init(sem);
	for (i = 0; i < atoi(argv[2]); i++) {
		children[i] = fork();
		if (children[i] == 0)
			spinner(sem, fair);
	}
	
	for (i = 0; i < atoi(argv[3]); i++) {
		struct timeval start, end, diff;

		sleep(1);
		gettimeofday(&start, NULL);
		futex_down(sem);
		gettimeofday(&end, NULL);
		futex_up(sem);
		timersub(&end, &start, &diff);
		printf("Wait time: %lu.%06lu\n", diff.tv_sec, diff.tv_usec);
		if (diff.tv_sec * 1000000 + diff.tv_usec > maxtime)
			maxtime = diff.tv_sec * 1000000 + diff.tv_usec;
	}

	/* Kill children */
	for (i = 0; i < atoi(argv[2]); i++)
		kill(children[i], SIGTERM);

	printf("Worst case: %lu\n", maxtime);
	exit(0);
}
