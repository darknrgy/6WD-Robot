#ifndef ANALOGINPUT_H
#define ANALOGINPUT_H

// initialize ADC converter
void analoginput_init();

// get 10 bit value by channel (pin number)
uint16_t analoginput_get(uint8_t chan);


#endif // #define ANALOGINPUT_H
