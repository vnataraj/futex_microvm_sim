#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "usersem.h"

#ifndef PROT_SEM
#define PROT_SEM 0x8
#endif

int main(int argc, char *argv[])
{
	struct futex *sem;
	int fd;
	void *map;
	unsigned int i;

	if (argc != 4) {
		fprintf(stderr, "Usage: test-dual <file> <offset> <iterations>\n");
		exit(1);
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("opening file");
		exit(1);
	}
	map = mmap(NULL, 4096, PROT_READ|PROT_WRITE|PROT_SEM, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	sem = map + atoi(argv[2]);

	for (i = 0; i < atoi(argv[3]); i++) {
		int j;

		for (j = random() % 1024; j > 0; j--);

		fprintf(stderr, "%i", getpid());
		futex_down(sem);
		fprintf(stderr, "v\n");
		/* Write pid here */
		*(volatile int *)(sem + 1) = getpid();
		for (j = random() % 4096; j > 0; j--) {
			if (*(volatile int *)(sem + 1) != getpid()) {
				printf("%u: ARGH!\n", getpid());
				exit(1);
			}
		}
		fprintf(stderr, "%i", getpid());
#if 1
		futex_up(sem);
#else
		futex_up_fair(sem);
#endif
		fprintf(stderr, "^\n");
	}
	exit(0);
}
	
