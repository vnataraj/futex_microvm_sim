#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "furwock.h"

int main(int argc, char *argv[])
{
	struct furwock *furwock;
	int fd;
	void *map;

	if (argc != 4) {
		fprintf(stderr, "Usage: testrw <file> <offset> show|init|upw|upr||downw|downr\n");
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
	furwock = map + atoi(argv[2]);
	if (strcmp(argv[3], "init") == 0)
		furwock_init(furwock);
	else if (strcmp(argv[3], "upw") == 0) {
		furwock_write_up(furwock);
	} else if (strcmp(argv[3], "downw") == 0) {
		if (furwock_write_down(furwock) != 0) {
			perror("furwock_write_down");
			exit(1);
		}
	} else if (strcmp(argv[3], "upr") == 0) {
		furwock_read_up(furwock);
	} else if (strcmp(argv[3], "downr") == 0) {
		if (furwock_read_down(furwock) != 0) {
			perror("furwock_read_down");
			exit(1);
		}
	} else if (strcmp(argv[3], "show") == 0) {
		printf("gate = %i\n", furwock->gate.count);
		printf("count = %i\n", furwock->count);
		printf("writer waiting = %s (%i)\n",
		       furwock->wait.count ? "YES" : "NO",
		       furwock->wait.count);
	}
	exit(0);
}
	
