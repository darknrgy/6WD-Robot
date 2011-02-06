#include <avr/io.h>
#include "../macros.h"
#include "../pwm.h"

// init PWM
void pwm_init(){
	
	// set  PORTB to output mode
	setbit(DDRD, BIT(DDD4) | BIT(DDD5));
    
    // set initial values
    OCR1A = 0x00; // channel 1
    OCR1B = 0x00; // channel 2
    
    // enable output compare match channel A intterupt
    //setbit(TIMSK, BIT(OCIE1A));
    
    // initialize timer
    // 8 bit PWM non-inverted
	setbit(TCCR1A, BIT(COM1A1) | BIT(COM1B1) | BIT(WGM10) | BIT(WGM11));
	
	// TCCR1A = 0xA1;
	
    // set scalar and kick off timer
    setbit(TCCR1B, BIT(CS10));
	//setbit(TCCR1B, 0x02);  
}

// modify the pwn prescalar on the fly
void pwm_set_scalar(char scalar){	
	char temp = (TCCR1B & 0xF8);
	TCCR1B =  temp| (scalar &= 0x07);
} 

// set a pwm channel to value
void pwm_set(char channel, uint16_t value){
    switch (channel){
        case PWM_CHAN_A: OCR1A = value; break;
        case PWM_CHAN_B: OCR1B = value; break;
        default: break;
    }
}
