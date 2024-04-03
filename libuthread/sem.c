#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"


struct semaphore {
	int count;
	queue_t blocked_queue;
};


sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(struct semaphore));

	// check if allocation failed
	if (sem == NULL) 
	{
		return NULL;
	}

	sem->blocked_queue = queue_create();
	if (sem->blocked_queue == NULL) 
	{
		return NULL;
	}

	sem->count = count;

	return sem;
}


int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->blocked_queue) > 0) 
	{
		return -1;
	}

	queue_destroy(sem->blocked_queue);
	free(sem);

	return 0;
}


int sem_down(sem_t sem)
{
	if (sem == NULL) 
	{
		return -1;
	}

	// start of critical section
	preempt_disable();

	// wait for avaliable resources
	while (sem->count == 0) 
	{
		queue_enqueue(sem->blocked_queue, uthread_current());
		uthread_block();
	}

	sem->count--;

	// end critical section
	preempt_enable();

	return 0;
}


int sem_up(sem_t sem)
{
	if (sem == NULL) 
	{
		return -1;
	}

	// start critical section
	preempt_disable();

	sem->count++;

	// if there is waiting thread, unblock it
	if (queue_length(sem->blocked_queue) > 0) 
	{
		struct uthread_tcb *unblock_thread;
		// dequeue a thread from blocked queue
		queue_dequeue(sem->blocked_queue, (void**)&unblock_thread);
		uthread_unblock(unblock_thread);
	}

	// end critical section
	preempt_enable();
	
	return 0;
}

