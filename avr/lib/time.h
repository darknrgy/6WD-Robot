#ifndef TIME_H
#define TIME_H

/* 
 * Timer
 * access a time value, examine time deltas, convert time to seconds, etc
 */
 
/* DONT FORGET TO PUT -lm in the linker options to support floating point math properly! */


#include <avr/interrupt.h>
#include <stdint.h>
#include "macros.h"


#define TIME_MAINTIMEOVERFLOW   1000000
#define TIME_TIMER_RESOLUTION   256

// modify these to use a different timer, eg: TCNT0, TCCR0, TOIE0 for timer0
#define TIME_PRECISION_TIMER    TCNT2
#define TIME_CONFIG             TCCR2
#define TIME_OVFLOW_INTERRUPT   TOIE2
#define	TIME_INTERRUPT_VECT		TIMER2_OVF_vect
#define TIME_SCALAR				8

typedef struct TimeResult{
    uint32_t delta;
    uint32_t previous;
} TimeResult;


/* 
 * initialize and kick off the timer
 */
void time_init();
 
/*
 * Interrupt
 * once ever 256 clock cycles
 * increment the main timer
 */
void interrupt();

/* 
 * get current time
 */
uint32_t time_get_time();

/* 
 * get a time delta, 
 * also returns current time so to not miss time with a subsequent time_get_time() call
 */
TimeResult time_get_time_delta(uint32_t previous);
 

/* 
 * view a time in seconds
 */
double get_time_in_ms(uint32_t time);

#endif // #ifndef TIME_H
