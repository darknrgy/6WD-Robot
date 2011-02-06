#ifndef QUEUEINT32_H
#define QUEUEINT32_H

#include <stdint.h>
#include <stdlib.h>

// queue_int32 Error Codes
#define QUEUE_ERROR_NOERROR 		0
#define QUEUE_ERROR_MALLOC 		1
#define QUEUE_ERROR_QUEUEFULL 	2
#define QUEUE_ERROR_QUEUEEMPTY 	3

// queue_int32
// the structure that holds all data for a queue
typedef struct Queue{
	uint8_t in;				// position for the next queued element
	uint8_t out;			// position for the next dequeued element
	uint8_t count;			// total number of items in the queue
	uint8_t size;			// maximum size of the queue
	uint8_t error;			// error code
	int32_t *queue;		// the data in the queue
} Queue;



// Create a new queue
Queue Queue_create(uint8_t queue_size);

// Enqueue a new value into the queue
void Queue_enqueue(Queue *instance, int32_t value);

// dequeue and return a value from the queue
int32_t Queue_dequeue(Queue *instance);

// get the current buffer size of the queue
uint8_t Queue_getCount(Queue *instance);

// return the queue error and clear the current queue error
uint8_t Queue_geteError(Queue *instance);

#endif //#ifndef QUEUEINT32_H
