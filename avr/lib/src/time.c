#include "../time.h"

/* 
 * Timer
 * access a time value, examine time deltas, convert time to seconds, etc
 */

/* 
 * get main timer address, used internally
 */
uint32_t* time_get_time_counter();

/* 
 * initialize and kick off the timer
 */
void time_init(){
    // initialize timer in normal mode with no scalar
    setbit(TIME_CONFIG, 2);
    
    // set overflow interrupt
    setbit(TIMSK, BIT(TIME_OVFLOW_INTERRUPT));
}
 
/*
 * Interrupt
 * once ever TIME_TIMER_RESOLUTION clock cycles
 * increment the main timer
 */
ISR(TIME_INTERRUPT_VECT, ISR_NOBLOCK){
	uint32_t* time = time_get_time_counter();
    *time = *time + 1;
    if (*time >= (uint32_t) TIME_MAINTIMEOVERFLOW){
		*time = 0;
	}
}

/* 
 * get current time
 */
uint32_t time_get_time(){
    uint32_t* time = time_get_time_counter();
    uint32_t buffer = 0;
    buffer = (*time << 8) + (uint32_t) TIME_PRECISION_TIMER;
    return buffer;    
}

/* 
 * get a time delta, 
 * also returns current time so to not miss time with a subsequent time_get_time() call
 */
TimeResult time_get_time_delta(uint32_t previous){
    TimeResult result;
    uint32_t current = time_get_time();
    if (current > previous) result.delta = current - previous;
    else result.delta = (uint32_t) TIME_MAINTIMEOVERFLOW * TIME_TIMER_RESOLUTION - previous + current;
    result.previous = current;
    return result;
}
 

/* 
 * view a time in seconds
 */
double get_time_in_ms(uint32_t time){
    if (time == 0) time = time_get_time();
    return (double) time / F_CPU * TIME_SCALAR * 1000;
}

/* 
 * get main timer address, used internally
 */
uint32_t* time_get_time_counter(){
    static uint32_t time = 0;
    return &time;
}
