#include <avr/io.h>
#include <avr/interrupt.h>
#include "../macros.h"
#include "../analoginput.h"

uint16_t ADC_RESULT = 0x00;


ISR(ADC_vect){
    ADC_RESULT = 0x01;
}

void analoginput_init(){
    
    // AVCC with external capacitor at AREF pin
	setbit(ADMUX, BIT(REFS0));	
	
	// ADC Enable
	setbit(ADCSRA, BIT(ADEN) | BIT (ADIE) | BIT(ADPS2) | BIT(ADPS1));// | BIT (ADPS0));
}

uint16_t analoginput_get(uint8_t chan){
    ADMUX = (ADMUX & 0xE0) | chan;
    setbit(ADCSRA, BIT(ADSC));
    for (uint8_t i = 0; i < 255; i ++){
		if (ADC_RESULT) break;
	}
    ADC_RESULT = 0x00;
    return ADC;
}


