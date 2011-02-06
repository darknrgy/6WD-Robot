#ifndef QUEUE_H
#define QUEUE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_ERROR_NOERROR 		0
#define QUEUE_ERROR_MALLOC 			1
#define QUEUE_ERROR_QUEUEFULL 		2
#define QUEUE_ERROR_QUEUEEMPTY 		3

// the structure that holds all data for a queue
typedef struct Queue{
	uint8_t in;				// position for the next queued element
	uint8_t out;			// position for the next dequeued element
	uint8_t count;			// total number of items in the queue
	uint8_t size;			// maximum size of the queue
	size_t element_size;	// element length in bytes
	uint8_t error;			// error code
	void* queue;			// the data in the queue
} Queue;



// Create a new queue
Queue Queue_create(uint8_t queue_size, size_t element_size);

// Enqueue a new value into the queue
void Queue_enqueue(Queue *instance, void* value);

// dequeue and return a value from the queue
void Queue_dequeue(Queue *instance, void* value);

// get the current buffer size of the queue
uint8_t Queue_getCount(Queue *instance);

// return the queue error and clear the current queue error
uint8_t Queue_getError(Queue *instance);

#endif //#ifndef QUEUE_H
