#include "globals.h"

float* globals_container(){
	static float globals[] = {

	0.015,				// 0: PID Integral Constant
	0.15,				// 1: PID Differential Constant
	0.35				// 2: RPM averaging weight
	};
	return globals;

}

float globals(uint8_t id){

	float *globals = globals_container();
	return globals[id];
}


void globals_set(uint8_t id, float value){
	float *globals = globals_container();
	globals[id] = value;	
}
