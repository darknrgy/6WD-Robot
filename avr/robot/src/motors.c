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

// automatically set motor direction based on rpm polarity
void motors_setdirection(char chan, uint8_t dir);

/*******************************/
/* PUBLIC FUNCTION DEFINITIONS */
/*******************************/

// initialize the motor system
void motors_init(){
	
	// configure external interrupts on INT0 and INT1
	setbit(MCUCR,	BIT(ISC01) 	| 	BIT(ISC11));
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
		motor->dir = 1;
		motor->counter = 0;
    }
}

// set a motor speed and direction by channel
void motors_set(uint8_t chan, int16_t rpm){
	Motor* motor = motors_get_motor(chan);
	motor->rpm_target = rpm;
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
	uint8_t chan, dir;
	int16_t rpm_buffer;
	

	for (chan = 0; chan < MOTORS_CHANS; chan ++){
		
		motor = motors_get_motor(chan);
        rpm_buffer = motor->rpm_target;
        
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
		rpm *= motor->dir;
		
		// avg the rpm delta
        motor->rpm_delta = motor->rpm_delta * (1.0 - globals(2))  + (rpm - motor->rpm_previous) * globals(2);		
		
		// determine the pwm acceleration rate
        pwm_acc = ((float) rpm_buffer - rpm) * globals(0) - motor->rpm_delta * globals(1);
		
		motor->pwm =  motor->pwm + pwm_acc;
		if (motor->pwm > globals(3) ) motor->pwm = globals(3);
		if (motor->pwm < -globals(3)) motor->pwm = -globals(3);
		
		if (rpm_buffer == 0) motor->pwm = 0;
		pwm_set(chan,floor(abs(motor->pwm)));

		dir = 0;

		if (motor->pwm > 0) dir = 1;

		motors_setdirection(chan, dir);
		
		motor->rpm_previous = rpm;
		motor->rpm_measured = rpm;
	}    
}


void motors_setdirection(char chan, uint8_t dir){

	switch (chan){
		case MOTORS_RIGHT:
			if (dir){
				setbit(PORTC, BIT(4));
				clearbit(PORTC, BIT(7));
			}else{
				setbit(PORTC, BIT(7));
				clearbit(PORTC, BIT(4));
			}
			break;
		case MOTORS_LEFT:
			if (dir){
				setbit(PORTC, BIT(3));
				clearbit(PORTC, BIT(0));
			}else{
				setbit(PORTC, BIT(0));
				clearbit(PORTC, BIT(3));
			}
			break;
		default:
			break;
	}

}

/*******************************/
/* INTERNAL ONLY FUNCTIONS     */
/*******************************/

// LEFT motor interrupt
ISR(INT0_vect, ISR_NOBLOCK){

	Motor* motor;	
	motor = motors_get_motor(MOTORS_LEFT);
	
	if (getbit(PINA, BIT(4))){
		motor->dir = 1;
	}else{
		motor->dir = -1;
	}
	motor->counter++;
}

// RIGHT motor interrupt
ISR(INT1_vect, ISR_NOBLOCK){	
	Motor* motor;	
	motor = motors_get_motor(MOTORS_RIGHT);
	
	if (getbit(PINA, BIT(5))){
		motor->dir = -1;
	}else{
		motor->dir = 1;
	}
	motor->counter++;
}

// calculate a counter delta, internal only 
InterruptCounterResult motors_get_interrupt_counter_result(uint8_t chan, uint16_t previous){

    
	InterruptCounterResult result;
	Motor* motor;	
	motor = motors_get_motor(chan);
	if (motor->counter >= previous) result.delta = motor->counter - previous;
	else result.delta = (uint16_t) INTERRUPT_COUNTER_MAX - previous + motor->counter;
	result.previous = motor->counter;
	return result;

	/*

    uint16_t* current = motors_get_counter(chan);
    if (*current >= previous) result.delta = *current - previous;
    else result.delta = (uint16_t) INTERRUPT_COUNTER_MAX - previous + *current;
    result.previous = *current;
    return result;
	*/

}
