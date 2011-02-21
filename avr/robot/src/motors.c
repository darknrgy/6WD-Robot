#include "motors.h"

/*******************************/
/* INTERNAL HEADERS            */
/*******************************/

// struct for dealing with interrupt deltas
typedef struct InterruptCounterResult{
	uint16_t delta;
	uint16_t previous;
} InterruptCounterResult;

// get an interrupt counter by channel
uint16_t* motors_get_counter(uint8_t chan);

// get a counter delta by channel
InterruptCounterResult motors_get_interrupt_counter_result(uint8_t chan, uint16_t previous);

/*******************************/
/* PUBLIC FUNCTION DEFINITIONS */
/*******************************/

// initialize the motor system
void motors_init(){
	
	// configure external interrupts on INT0 and INT1
	setbit(MCUCR,	BIT(ISC00) 	| 	BIT(ISC10));
	setbit(GICR, 	BIT(INT0) 	| 	BIT(INT1));

	// initialize hardware PWM
	pwm_init();

	// set pwm scalar
	pwm_set_scalar(3);
    
    // init all the motor structs
    Motor* motor;
    for (uint8_t chan = 0; chan < MOTORS_CHANS; chan ++){
        motor = motors_get_motor(chan);
        motor->rpm_target = 0;
        motor->rpm_measured = 0;
        motor->rpm_previous = 0;
        motor->rpm_delta = 0;
        motor->pwm = 0;
        motor->dir_target = 0;
        motor->dir_measured = 0;        
    }
}

// set a motor speed and direction by channel
void motors_set(uint8_t chan, uint8_t dir, uint16_t rpm){
	Motor* motor = motors_get_motor(chan);
	motor->rpm_target = rpm;
	motor->dir_target = dir;
}


// get a motor by channel
Motor* motors_get_motor(uint8_t chan){
	static Motor motors[MOTORS_CHANS];
	return &motors[chan];
}

// calculate a time frame
void motors_tick(){

	Motor* motor;	
	static InterruptCounterResult icr[MOTORS_CHANS];
    static TimeResult tr[MOTORS_CHANS];
	float time_ms, pwm_acc, rpm;
	uint8_t chan;

	for (chan = 0; chan < MOTORS_CHANS; chan ++){
		
		motor = motors_get_motor(chan);
        uint16_t rpm_buffer = motor->rpm_target;
        if (motor->rpm_target > motor->rpm_measured){
            if (motor->rpm_target - motor->rpm_measured > MOTOR_JERK_BUFFER){
                rpm_buffer = motor->rpm_measured + MOTOR_JERK_BUFFER;
            }        
        }else if(motor->rpm_target < motor->rpm_measured){
            if (motor->rpm_measured - motor->rpm_target > MOTOR_JERK_BUFFER){
                rpm_buffer = motor->rpm_measured - MOTOR_JERK_BUFFER;
            }        
        }
        

		// get time delta
		tr[chan] = time_get_time_delta(tr[chan].previous);
		
		// get interrupt counter delta
		icr[chan] = motors_get_interrupt_counter_result(chan, icr[chan].previous);
		

		// 60000 is for converting to milliseconds to minutes (for RPM)
        time_ms = get_time_in_ms(tr[chan].delta);
		rpm =  (float) icr[chan].delta / (float) INTERRUPTS_PER_ROTATION / (float) time_ms * (float) 60000;
		
		// avg the rpm delta
        motor->rpm_delta = motor->rpm_delta * 0.65 + (rpm - motor->rpm_previous) * 0.35;		
		
		// determine the pwm acceleration rate
        pwm_acc = ((float) rpm_buffer - rpm) * 0.015 - motor->rpm_delta * 0.15;
		
		if (pwm_acc < - motor->pwm) motor->pwm = 0;
		else if (pwm_acc + motor->pwm > 1023) motor->pwm = 1023;
		else{
			 motor->pwm =  motor->pwm + pwm_acc;
		}

		
		if (rpm_buffer == 0) motor->pwm = 0;
		pwm_set(chan,floor(motor->pwm));
		motor->rpm_previous = rpm;
		motor->rpm_measured = rpm;
	}    
}

/*******************************/
/* INTERNAL ONLY FUNCTIONS     */
/*******************************/

// LEFT motor interrupt
ISR(INT0_vect, ISR_NOBLOCK){	
	uint16_t* counter = motors_get_counter(MOTORS_LEFT);
    *counter = *counter + 1;
}

// RIGHT motor interrupt
ISR(INT1_vect, ISR_NOBLOCK){	
	uint16_t* counter = motors_get_counter(MOTORS_RIGHT);
    *counter = *counter + 1;
}

// get an interrupt counter by channel
uint16_t* motors_get_counter(uint8_t chan){
	static uint16_t counters[2];
	return &counters[chan];

}

// calculate a counter delta, internal only 
InterruptCounterResult motors_get_interrupt_counter_result(uint8_t chan, uint16_t previous){

    InterruptCounterResult result;
    uint16_t* current = motors_get_counter(chan);
    if (*current >= previous) result.delta = *current - previous;
    else result.delta = (uint16_t) INTERRUPT_COUNTER_MAX - previous + *current;
    result.previous = *current;
    return result;
}
