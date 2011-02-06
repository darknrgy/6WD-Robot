#ifndef PWM_H
#define PWM_H


// channel masks
#define PWM_CHAN_A 0x00
#define PWM_CHAN_B 0x01


// init PWM
void pwm_init();

// set a pwm channel to value
void pwm_set(char channel, uint16_t value);

// modify the pwn prescalar on the fly
// 1: 1
// 2: 8
// 3: 64
// 4: 256
// 5: 1024
void pwm_set_scalar(char scalar);

#endif // #define PWM_H
