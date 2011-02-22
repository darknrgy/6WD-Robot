#ifndef MOTORS_H
#define MOTORS_H

#include "includes.h"

/* Dual Motor Controller 
 * Designed for TerraRover 1.0; All Terrain Robot
 */


#define MOTORS_LEFT			0
#define MOTORS_RIGHT		1
#define MOTORS_CHANS		2
#define INTERRUPT_COUNTER_MAX 65535
#define INTERRUPTS_PER_ROTATION 32
#define MOTOR_JERK_BUFFER   700


typedef struct Motor{
	
	// target RPM
	int16_t rpm_target;

	// measured RPM
	float rpm_measured;

	// previous measured RPM
	float rpm_previous;

	// averaged RPM delta
	float rpm_delta;
    
    // current PWM value
    float pwm;

} Motor;


// initialize the motor system
void motors_init();

// set a motor speed and direction by channel
void motors_set(uint8_t chan, int16_t rpm);

// get a motor by channel
Motor* motors_get_motor(uint8_t chan);

// calculate a time frame
void motors_tick();


#endif // #ifndef MOTORS_H
