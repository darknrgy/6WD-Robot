#include "globals.h"

float globals(uint8_t id){

	static float globals[] = {

	0.015,				// 0: PID Integral Constant
	0.15,				// 1: PID Differential Constant
	0.35				// 2: RPM averaging weight
	};

	return globals[id];
}
