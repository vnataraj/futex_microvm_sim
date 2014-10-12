LIBS=usersem.o furwock.o non-pthreads.o
ARCH=$(shell uname -m | sed 's/i.86/i386/')
KERNEL_DIR=/home/rusty/working-2.4.18-afutex/include

CC=gcc
CFLAGS=-Wall -g -I$(KERNEL_DIR)

all: $(LIBS) test test-dual testrw starve

starve: starve.o usersem.o

testrw: testrw.o $(LIBS)

test: test.o $(LIBS)

test-dual: test-dual.o $(LIBS)

usersem.o: usersem.c usersem.h $(ARCH).h
furwock.o: furwock.c furwock.h usersem.h $(ARCH).h
non-pthreads.o: non-pthreads.c usersem.h $(ARCH).h

clean:
	rm -f $(LIBS) test test-dual starve testrw *.o core
