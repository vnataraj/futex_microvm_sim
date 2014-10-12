#include "furwock.h"

void furwock_init(struct furwock *rwock)
{
	futex_init(&rwock->gate);
	/* 0 means "completely unlocked" */
	rwock->count = 0;
	futex_init(&rwock->wait);
	futex_down(&rwock->wait);
}
