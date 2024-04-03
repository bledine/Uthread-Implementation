#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;
	printf("thread3\n");
	exit(EXIT_SUCCESS);
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	printf("thread2\n");

	// attempts on take cpu forever
	while(1){}
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	printf("thread1\n");

	// attempts on take cpu forever
	while(1){}
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	return 0;
}