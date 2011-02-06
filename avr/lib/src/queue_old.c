#include "../queue.h"

typedef Packet QueueElement;


// Create a new queue
Queue Queue_create(uint8_t queue_size){
	
	// initialize queue
	Queue instance ;
	instance.in = 0;
	instance.out = 0;
	instance.count = 0;
	instance.size = queue_size;
	instance.error = QUEUE_ERROR_NOERROR;
	instance.queue = (int32_t*) malloc(queue_size * sizeof(int32_t));

	// check for malloc errors
	if (instance.queue == NULL){ 
		instance.error =  QUEUE_ERROR_MALLOC;
		return instance;
	}
	
	return instance;

}

// Enqueue a new value into the queue
void Queue_enqueue(Queue *instance, int32_t value){

	// check for overflowed queue
	if (instance->count + 1 > instance->size){
		instance->error = QUEUE_ERROR_QUEUEFULL;
		return;
	}

	// assign new queue value
	if (instance->in > (instance->size - 1) ) instance->in = 0;
	instance->queue[instance->in] = value;
	instance->in ++;	
	instance->count ++;

}

// dequeue and return a value from the queue
int32_t Queue_dequeue(Queue *instance){

	static uint8_t temp;
	if (instance->count == 0){
		instance->error = QUEUE_ERROR_QUEUEEMPTY;
		return 0x00;
	}
	temp = instance->out;
	if (++instance->out > (instance->size - 1) ) instance->out = 0;
	instance->count--;
	return instance->queue[temp];
}

// get the current buffer size of the queue
uint8_t Queue_getCount(Queue *instance){
	return instance->count;
}

// return the queue error and clear the current queue error
uint8_t Queue_geteError(Queue *instance){
	static uint8_t error;
	error = instance->error;
	instance->error = QUEUE_ERROR_NOERROR;
	return error; 
}

