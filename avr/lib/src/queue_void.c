#include "../queue.h"



// Create a new queue
Queue Queue_create(uint8_t queue_size, uint8_t element_size){
	
	// initialize queue
	Queue instance ;
	instance.in = 0;
	instance.out = 0;
	instance.count = 0;
	instance.size = queue_size;
	instance.element_size = element_size;
	instance.error = QUEUE_ERROR_NOERROR;
	instance.queue = (QueueElement*) calloc(queue_size * element_size);


	// check for malloc errors
	if (instance.queue == NULL){ 
		instance.error =  QUEUE_ERROR_MALLOC;
		return instance;
	}
	
	return instance;

}

// Enqueue a new value into the queue
void Queue_enqueue(*Queue instance, void* value){

	// check for overflowed queue
	if (instance->count + 1 > instance->size){
		instance->error = QUEUE_ERROR_QUEUEFULL;
		return;
	}

	// assign new queue value
	if (instance->in > (instance->size - 1) ) instance->in = 0;

	memcpy(&instance->queue[(instance->in * instance->element_size)], value, instance->element_size);
	//instance->queue[instance->in] = value;
	instance->in ++;	
	instance->count ++;

}

// dequeue and return a value from the queue
void Queue_dequeue(Queue *instance, void* value){

	static uint8_t temp;
	if (instance->count == 0){
		instance->error = QUEUE_ERROR_QUEUEEMPTY;
		//static QueueElement element;
		//return NULL;
		value = NULL;
		return;
	}
	temp = instance->out;
	if (++instance->out > (instance->size - 1) ) instance->out = 0;
	instance->count--;
	//return &instance->queue[temp];
	memcpy(value, &instance->queue[instance->in*instance->element_size], instance->element_size);
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

