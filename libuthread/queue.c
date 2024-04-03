#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"



typedef struct node 
{
	//void so any data type can be stored
	void *data;
	struct node *next;
} node;

struct queue 
{
	node *head;
	node *tail;
	int size;
};


queue_t queue_create(void)
{
	queue_t q = malloc(sizeof(queue_t));

	/*check if allocation failed and return NULL*/
	if (q == NULL) 
	{
		return NULL;
	}

	q->head = NULL;
	q->tail = NULL;
	q->size = 0;

	return q;
}


int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->size != 0) 
	{
		return -1;
	}

	free(queue);		//dealocate queue
	return 0;
}


int queue_enqueue(queue_t queue, void *data)
{	
	// subject queue/data to enqueue is not allocated 
	if (queue == NULL || data == NULL)
	{
		return -1;
	}

	node *newNode = malloc(sizeof(node));

	// check if allocation failed
	if (newNode == NULL) 
	{
		return -1;
	}

	// initializaiton
	newNode->data = data;
	newNode->next = NULL;

	// check if the new node will be the first one
	if (queue->tail == NULL) 
	{
		queue->head = newNode;
	} else {
		queue->tail->next = newNode;
	}

	// update queue
	queue->tail = newNode;
	queue->size++;
	return 0;
}


int queue_dequeue(queue_t queue, void **data)
{
	// non-allocated queue or empty queue
	if (queue == NULL || queue->head == NULL || data == NULL) 
	{
		return -1;
	}

	node *nodeTemp = queue->head;
	*data = nodeTemp->data;

	// remove first node
	queue->head = queue->head->next;

	// check if there is only one node in queue after dequeue
	if (queue->head == NULL)
	{
		queue->tail = NULL;
	}

	// update queue
	free(nodeTemp);
	queue->size--;
	return 0;
}


int queue_delete(queue_t queue, void *data)
{
	// check if queue or data are NULL or if queue is empty
	if (queue == NULL || queue->size == 0 || data == NULL) 
	{
		return -1;
	}

	node *currentNode = queue->head;
	node *previousNode = NULL;

	while (currentNode != NULL && currentNode->data != data) 
	{
		if (currentNode->data == data) {
			break;
		}
		previousNode = currentNode;
		currentNode = currentNode->next;
	}

	// check if a match was found in queue
	if (currentNode != NULL) 
	{
		// check if match was head
		if (previousNode == NULL) 
		{
			queue->head = currentNode->next;

			// if the queue is empty after deletion
			if (queue->head == NULL)
			{
				queue->tail = NULL;
			}
		} else {
			previousNode->next = currentNode->next;
		}

		// check if match was tail
		if (currentNode == queue->tail) 
		{
			queue->tail = previousNode;
		}

		free(currentNode);		// dealocate node
		queue->size--;		// decrease size by 1
		return 0;
	}

	return -1;
}


int queue_iterate(queue_t queue, queue_func_t func)
{	
	// check if NULL queue or func not defined
	if (queue == NULL || func == NULL)
	{
		return -1;
	}

	// iterate through and call func on each nodes of the queue
	node *currentNode = queue->head;
	node *nextNode;
	while (currentNode != NULL) 
	{
		nextNode = currentNode->next;
		func(queue, currentNode->data);
		currentNode = nextNode;
	}

	return 0;
}


int queue_length(queue_t queue)
{
	if (queue == NULL) 
	{
		return -1;
	}

	return queue->size;
}
