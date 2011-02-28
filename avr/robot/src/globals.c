#include "globals.h"

#define TOTAL_GLOBALS 4

float* globals_container(){
	static float globals[TOTAL_GLOBALS];
	return globals;

}

void globals_init(){
	float *globals = globals_container();
	globals[0] = 0.015;			// 0: PID Integral Constant
	globals[1] = 0.15;				// 1: PID Differential Constant
	globals[2] = 0.35;				// 2: RPM averaging weight
	globals[3] = 512.0;				// 3: max PWM
}

float globals(uint8_t id){
	if (id > TOTAL_GLOBALS-1) return 0.0f;
	float *globals = globals_container();
	return globals[id];
}


void globals_set(uint8_t id, float value){
	if (id > TOTAL_GLOBALS-1) return;
	float *globals = globals_container();
	globals[id] = value;	
}
