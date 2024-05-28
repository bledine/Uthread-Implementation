# Project 2: User-level thread library
## Summary
This project implements a basic user-level thread library for Linux. The library 
provides a complete interface for applications to create and run independent 
threads concurrently. We implemented a queue library, uthread library, 
semaphores and preemption funcitons in 4 phases. Few testing scripts are also 
added to test the functionality of each library.

# Implementation
## Makefile
The Makefile compiles the target library libuthread.a from object files 
(queue.o, uthread.o, context.o, preempt.o, sem.o). It includes flags for 
warnings and error checking during compilation. Dependency tracking ensures 
file changes trigger recompilation. The clean rule removes generated files. 
Verbosity control **($(V))** toggles output visibility.

## libuthread
### Phase 1 **queue.c**
In this phase, we implemented a simple FIFO queue, it defines two data
structure, **node(type)** and **queue**.
**node(type)**
The node structure type has member **data** that is a void pointer so it can 
store any data type, and a member **next** that points to the next node in a 
linked list fasion.
**queue**
The queue structure is used to store the first(head) and last(tail) nodes of a 
queue, and has a memeber **size** to indicate the length of the queue.
Queues are created by **queue_create()** that allocates space required for new 
queue, it sets head and tail to NULL and size to 0, and return the address of 
the new queue. The function returns the new queue, but if queue allocation 
fails, it returns NULL. The newly created queue can be operated by: 
* **queue_enqueue()**: if the given queue and data is not NULL, store data in 
newly allocated *node* and add to the end of queue, then adjust the enqueued 
node as tail, 
* **queue_dequeue()**:  if the given queue and data output address pointer is 
not NULL, removes node from the head of queue, retrive the data, free the 
removed node and set the next node as * head,then if the head is NULL (queue 
become empty after dequeue) set tail to NULL.
* **queue_destroy()**: deallocates a **existing empty** queue;
* **queue_delete()**: if the queue and data are valid, search a node by 
specified data, removes(free) it and connect the queue again properly.
* **queue_iterate()**: if given queue and func is valid, iterate through every 
node in the queue and calls provided func on each nodes' data.
* **queue_length()**: simply returns the length of the queue if its not NULL; <br>

#### *Testing*
A new test script is added to /APP name *queue_tester.c* based on 
*queue_tester_example.c* that covers various cases on usage of the queue library
The *queue_tester.c* tests:
* Create queue
* NULL data Enqueue/Dequeue
* Single Enqueue/Dequeue
* Multiple Enqueue/Dequeue
* Queue length return 
* Dequeue empty queue
* Destroy empty queue
* Destroy non-empty queue
* Queue delete given 1 data
* Queue delete given NULL data 
* Queue iterate with delele function
* Queue iterate with NULL function

### Phase 2 **uthread.c**
The uthread.c file manages the creation, execution, and destruction of 
user-level threads in the program. This implementation uses queues to organize 
threads based on their state and uses context switching mechanisms to facilitate 
thread execution and context management.
Key components and functionalities include:
* **Global Queues:** Two queues, ready_queue and exited_queue, manage uthread 
states and lifecycle.
* **uthread_tcb struct** stores the thread control block for each thread. It 
contains the state of the thread and a pointer to the threads context and stack.
* **uthread_current** returns the currently executing thread.
* **uthread_destroy** deallocates resources associated with a thread upon its 
exit.
* **uthread_yield** yields the CPU, allowing another ready thread to execute.
* **uthread_exit** terminates the current thread, transferring control to the 
next ready thread if there is any.
* **uthread_create** creates a new thread, allocating memory for its stack and 
context, and initializes it for execution.
* **uthread_run** initializes the idle thread, creates an initial thread, and 
manages the execution of threads until completion.
* **uthread_block** blocks the current thread, allowing another ready thread to 
execute.
* **uthread_unblock** unblocks a specified thread, moving it to the ready state 
for execution.

#### *Testing*
To test the implementation of **uthread.c** we used the two provided test cases. 
The tests cover scenarios involving thread creation, context switching, and 
cooperative multitasking, ensuring the reliability and correctness of the user-
level threading library.
The provided test cases include:
* **uthread_hello**, a simple test case that evaluates the successful creation 
and execution of a single thread. The thread prints "Hello world!" and returns.
* **uthread_yield** evaluates the creation and yielding of threads. Here 
multiple threads are created in a one after the other, and each thread yields 
control to the next one.

### Phase 3 **sem.c**
The **sem.c** file provides an implementation for semaphore operations in the 
User-level thread library. The implementation ensures thread safety and proper 
resource management, adhering to the specifications outlined in the semaphore 
API.

The **struct semaphore** contains the semaphore's count and a blocked queue to 
manage threads waiting for resources.
The function **sem_create** creates a semaphore with an initial count specified 
by the parameter **count**. It allocates memory for the semaphore structure and 
initializes the blocked queue. If memory allocation fails, it returns NULL. The 
function **sem_destroy** function deallocates the semaphore and the blocked 
queue if they no more threads waiting in there.

The **sem_down** function decrements the semaphore's count and blockes the 
current thread is resources are not available. It enqueues the current thread 
into the blocked queue and blockes it until resources become available. The 
**sem_up** function increments the semaphore's count, signaling that resources 
are available. It dequeues a thread on the blocked queue if the there are any.

#### *Testing*
To test the implementation of **sem.c**, we used four provided test programs 
that tested specific aspects of the semaphore synchronization. 
The test programs include:
* **sem_simple.c** evaluates the basic functionality of semaphores with three 
threads. It verifies that the threads print their message in the proper order.
* **sem_count.c** evaluates the synnchronization of two threads that print 
numbers from 0 to a specified maximum count in an orderly fashion, one number 
per thread at a time.
* **sem_prime.c** implements a sieve algorithm for finding prime numbers. It 
makes sure the producer correctly generates numbers, filters out non-prime 
numbers, and the consumer accurately identifies prime numbers from the pipeline.
* **sem_buffer.c** evaluates the synchronization between producers and consumers 
using semaphores. It verifies whether the producer correctly inserts items into 
a shared buffer, and consumers can consume items without data corruption or race 
conditions.

### Phase 4 **preempt.c**
This implementation of preemption mechanism allows the uthread library to 
enable/disable preempt in critical sessions, manage the threads in 
**preempt == true** mode that interupt threads a hundred times per second, and 
stop the preempt mode.

Two static global variable **prev_sa** and **prev_tc** are used to store the 
previous signal action and timer configuration when enters **preempt == true** 
mode.

When a peice of code requites no interupt by the preempt, it first call 
**preempt_disable()** to temporarily **block SIGVTALRM** signal delivery, 
disable the preemption, when the lines in critical sessions are executed, 
**preempt_enable()** is called to **unblock SIGVTALRM** signal, enabling the 
preemption again.

Whenever in **preempt == true** mode, the **preempt_start()** will initialize 
the preemption by save the current signal action and timer configuration 
**prev_sa** and **prev_tc**, then set up a signal handler for **SIGVTALRM** 
using sigaction and configures a virtual timer (**ITIMER_VIRTUAL**) to trigger 
SIGVTALRM 100 times per second that calls the **signal_handler()** which 
performs **uthread_yield()** to interupt the working thread. If the preemption 
ends, **preempt_stop()** will restore the previous saved signal action and 
timer configuration from *prev_sa** and **prev_tc**.

#### *Testing*
A new test script is added to /APP name **test_preempt.c** based on 
**uthread_yield.c** that tests the uthread library with preempt, in this 
script 3 thread will be created and the first two thread will run into 
while(1){} loop that tries to take the cpu forever. However, as the preempt 
mode is on, these two thread will be interupted and the process will be 
terminated by thread 3 using exit().
