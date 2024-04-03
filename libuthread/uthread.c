#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

// global ready queue
queue_t ready_queue;

// global exited queue
queue_t exited_queue;

struct uthread_tcb* current_thread;

// define for thread states
typedef enum {
	READY,
	RUNNING,
	BLOCKED,
	EXITED
} uthread_state;


struct uthread_tcb {
	uthread_ctx_t *context;
	void *stack;
	uthread_state state;
};


struct uthread_tcb *uthread_current(void)
{
	return current_thread;
}


// deallocated threads
void uthread_destroy(queue_t q, void *data)
{
	preempt_disable();

	struct uthread_tcb* thread = (struct uthread_tcb*) data;

	free(thread->context);
	uthread_ctx_destroy_stack(thread->stack);
	free(thread);
	queue_delete(q, thread);

	preempt_enable();
}


void uthread_yield(void)
{	
	// critical block
	preempt_disable();

	// get the thread that yields
	struct uthread_tcb *yield_thread = uthread_current();

	// set current thread back to ready queue from running to ready
	if (yield_thread->state == RUNNING) 
	{
		yield_thread->state = READY;
		queue_enqueue(ready_queue, yield_thread);
	}

	// get next ready thread running
	struct uthread_tcb *next_thread;
	if (queue_dequeue(ready_queue, (void **)&next_thread) == -1) {
		return;
	}
	if (next_thread->state == READY)
	{
		next_thread->state = RUNNING;
		current_thread = next_thread;
	}

	// save yield context and active next thread context
	uthread_ctx_switch(yield_thread->context, current_thread->context);

	// critical block end
	preempt_enable();
}


void uthread_exit(void)
{
	// start of critical section
	preempt_disable();

	struct uthread_tcb *exited_thread = uthread_current();

	exited_thread->state = EXITED;
	queue_enqueue(exited_queue, exited_thread);

	// get next ready thread running
	struct uthread_tcb *next_thread;

	if (queue_dequeue(ready_queue, (void **)&next_thread) == -1)
	{
		return;
	}

	if (next_thread->state == READY)
	{
		next_thread->state = RUNNING;
		current_thread = next_thread;
	}

	uthread_ctx_switch(exited_thread->context, current_thread->context);

	// end of critical section
	preempt_enable();

}


int uthread_create(uthread_func_t func, void *arg)
{
	// start of critical section
	preempt_disable();

	struct uthread_tcb *thread = malloc(sizeof(struct uthread_tcb));
	// check of memory allocation failed
	if (thread == NULL)
	{
		return -1;
	}

	thread->stack = uthread_ctx_alloc_stack();

	// check of stack creation failed
	if (thread->stack == NULL)
	{
		preempt_enable();
		free(thread);
		return -1;
	}

	thread->context = malloc(sizeof(uthread_ctx_t));
	// check if allocation failed
	if (thread->context == NULL)
	{
		preempt_enable();
		uthread_ctx_destroy_stack(thread->stack);
		free(thread);
		return -1;
	}
	
	// check of context creation failed
	if (uthread_ctx_init(thread->context, thread->stack, func, arg) == -1)
	{
		uthread_ctx_destroy_stack(thread->stack);
		free(thread);
		preempt_enable();
		return -1;
	}

	// update state
	thread->state = READY;
	// add thread to queue
	queue_enqueue(ready_queue, thread);

	// end of critical section
	preempt_enable();
	return 0;
}


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{	
	preempt_start(preempt);

	//start critical section
	preempt_disable();

	// initialize the ready queue and check if it was successful
	ready_queue = queue_create();
	if (ready_queue == NULL)
	{
		return -1;
	}

	exited_queue = queue_create();
	if (exited_queue == NULL)
	{
		return -1;
	}

	struct uthread_tcb *idle_thread;
	uthread_create(NULL, NULL);
	queue_dequeue(ready_queue, (void **)&idle_thread);

	current_thread = idle_thread;

	// create initial thread
	if (uthread_create(func, arg) == -1)
	{
		return -1;
	}

	// end critical section
	preempt_enable();

	while (1) {
		// check if ready queue is empty
		if (queue_length(ready_queue) == 0)
		{
			break;
		}
		uthread_yield();

		// delete exited threads
		queue_iterate(exited_queue, uthread_destroy);
	}

	if (preempt) {
		preempt_stop();
	}

	queue_destroy(exited_queue);
	queue_destroy(ready_queue);
	
	return 0;
}

void uthread_block(void)
{
	// block the current thread
	struct uthread_tcb *prev_thread = current_thread;
	prev_thread->state = BLOCKED;

	// dequeue fails or no more items in ready queue
	if (queue_dequeue(ready_queue, (void **)&current_thread) == -1)
	{
		return;
	}

	current_thread->state = RUNNING;

	// save yield context and active next thread context
	uthread_ctx_switch(prev_thread->context, current_thread->context);

}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
	queue_enqueue(ready_queue, uthread);
}

