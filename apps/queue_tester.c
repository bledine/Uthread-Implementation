#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Single Enqueue/Dequeue */
void test_queue_EnDequeue_single(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_EnDequeue_single ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);

	TEST_ASSERT(ptr == &data);
}

/* Test NULL data Enqueue/Dequeue */
void test_queue_EnDequeue_NULL(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST queue_EnDequeue_NULL ***\n");

	q = queue_create();
	TEST_ASSERT(queue_enqueue(q, NULL) == -1);
	TEST_ASSERT(queue_dequeue(q, NULL) == -1);
}

/* Multiple Enqueue/Dequeue */
void test_queue_EnDequeue_multiple(void)
{	
	queue_t q;
	int data1 = 1;
	int data2 = 2;
	int *ptr1, *ptr2;

	fprintf(stderr, "*** TEST queue_EnDequeue_multiple ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_dequeue(q, (void**)&ptr1);
	queue_dequeue(q, (void**)&ptr2);

	TEST_ASSERT(ptr1 == &data1 && ptr2 == &data2);
}

/* Queue length */
void test_queue_length(void)
{
	queue_t q;
	int data1 = 1;
	int data2 = 2;

	fprintf(stderr, "*** TEST queue_length ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);

	TEST_ASSERT(queue_length(q) == 2);
}

/* Empty queue */
void test_queue_empty(void) 
{
	queue_t q;
	q = queue_create();
	int *ptr = NULL;
	fprintf(stderr, "*** TEST queue_empty ***\n");
	int res = queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(res == -1 && ptr == NULL);
}

/* Destroy empty queue */
void test_queue_destroy_empty(void)
{
	queue_t q;
	q = queue_create();

	fprintf(stderr, "*** TEST queue_destroy_empty ***\n");

	TEST_ASSERT(queue_destroy(q) == 0);
}

/* Destroy non-empty queue */
void test_queue_destroy_nonEmpty(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_destroy_nonEmpty ***\n");

	q = queue_create();
	queue_enqueue(q, &data);

	TEST_ASSERT(queue_destroy(q) == -1);
}

/* Test queue delete given 1 data */
void test_queue_delete(void)
{
	int data = 3;
	int data2 = 4;
	queue_t q;

	fprintf(stderr, "*** TEST queue_queue_delete ***\n");

	q = queue_create();
	queue_enqueue(q, &data);

	TEST_ASSERT(queue_delete(q, &data) == 0);
	TEST_ASSERT(queue_delete(q, &data2) == -1);
	TEST_ASSERT(queue_length(q) == 0);
}

/* Test queue delete given NULL data */
void test_queue_delete_NULL(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST queue_queue_delete_NULL ***\n");

	q = queue_create();

	TEST_ASSERT(queue_delete(q, NULL) == -1);
}

/* Test queue iterate with delele function */
void delete(queue_t q, void* data) {
	queue_delete(q, data);
}

void test_queue_iterate_delete(void) {
    queue_t q;

	fprintf(stderr, "*** TEST test_queue_iterate_delete ***\n");

	q = queue_create();

    int data1 = 10, data2 = 20, data3 = 30;
    queue_enqueue(q, &data1);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);

	TEST_ASSERT(queue_length(q) == 3);

	TEST_ASSERT(queue_iterate(q, delete) == 0);
	TEST_ASSERT(queue_destroy(q) == 0);
}

/* Test queue iterate with NULL function */
void test_queue_iterate_NULL(void) {
    queue_t q;

	fprintf(stderr, "*** TEST test_queue_iterate_NULL ***\n");

	q = queue_create();

	TEST_ASSERT(queue_iterate(q, NULL) == -1);
	TEST_ASSERT(queue_destroy(q) == 0);
}

int main(void)
{
	test_create();
	test_queue_EnDequeue_single();
	test_queue_EnDequeue_NULL();
	test_queue_EnDequeue_multiple();
	test_queue_length();
	test_queue_empty();
	test_queue_destroy_empty();
	test_queue_destroy_nonEmpty();
	test_queue_delete();
	test_queue_delete_NULL();
	test_queue_iterate_delete();
	test_queue_iterate_NULL();

	printf("All tests executed.\n");
	return 0;
}
