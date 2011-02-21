#ifndef INCLUDES_H
#define INCLUDES_H

// system includes
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>

// library includes
#include "../../lib/macros.h"
#include "../../lib/queue.h"
#include "../../lib/packets.h"
#include "../../lib/crc16.h"
#include "../../lib/usart.h"
#include "../../lib/crc16.h"
#include "../../lib/pwm.h"
#include "../../lib/time.h"
#include "../../lib/analoginput.h"

// application includes
#include "commands.h"
#include "motors.h"


#endif // #ifndef INCLUDES_H

