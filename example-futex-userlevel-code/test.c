#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include "usersem.h"

#ifndef F_SETSIG
#define F_SETSIG 10
#endif

static void sigio_action(int signal, siginfo_t *info, void *unused)
{
	fprintf(stderr, "signal %i, fd %i\n", signal, info->si_fd);
}

int main(int argc, char *argv[])
{
	struct futex *futex;
	int fd;
	void *map;

	if (argc != 4) {
		fprintf(stderr, "Usage: test <file> <offset> show|init|up|upfair|down|down1sec|mem|async\n");
		exit(1);
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror("opening file");
		exit(1);
	}
	map = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (map == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	if (futex_region(map, 4096) != 0) {
		perror("futex_region");
		exit(1);
	}
	futex = map + atoi(argv[2]);
	if (strcmp(argv[3], "init") == 0)
		futex_init(futex);
	else if (strcmp(argv[3], "up") == 0) {
		int woken;
		woken = futex_up(futex);
		if (woken < 0) {
			perror("futex_up");
			exit(1);
		}
		printf("Woke %i\n", woken);
	} else if (strcmp(argv[3], "upfair") == 0) {
		int woken;
		woken = futex_up_fair(futex);
		if (woken < 0) {
			perror("futex_up");
			exit(1);
		}
		printf("Woke %i\n", woken);
	} else if (strcmp(argv[3], "down") == 0) {
		if (futex_down(futex) != 0) {
			perror("futex_down");
			exit(1);
		}
	} else if (strcmp(argv[3], "down1sec") == 0) {
		struct timespec t;
		t.tv_sec = 1;
		t.tv_nsec = 0;
		if (futex_down_timeout(futex, &t) != 0) {
			perror("futex_down_timeout");
			exit(1);
		}
	} else if (strcmp(argv[3], "mem") == 0) {
		struct futex myfutex;
		futex_init(&myfutex);
		futex_down(&myfutex);
		futex_down(&myfutex);
	} else if (strcmp(argv[3], "show") == 0) {
		printf("count = %i\n", futex->count);
	} else if (strcmp(argv[3], "async") == 0) {
		static struct sigaction saction;
		int futex_fd;

		/* Set up SIGIO handler */
		saction.sa_sigaction = sigio_action;
		saction.sa_flags = SA_SIGINFO|SA_ONESHOT;

		while (futex_trydown(futex) != 0) {
			/* Register for signal */
			sigaction(SIGIO, &saction, NULL);
			futex_fd = futex_await(futex, SIGIO);
			if (futex_fd < 0) {
				perror("await");
				exit(1);
			}
			if (futex_trydown(futex) == 0) {
				close(futex_fd);
				break;
			}
			pause();
			close(futex_fd);
			/* <= in case someone else decremented it */
			if (futex->count <= FUTEX_PASSED) {
				futex->count = -1;
				fprintf(stderr, "Futex was passed to us.\n");
				break;
			}
		}
	} else {
		fprintf(stderr, "Unknown operation %s\n", argv[3]);
		exit(1);
	}
		
	exit(0);
}
	
