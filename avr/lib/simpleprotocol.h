#ifndef SIMPLEPROTOCOL_H
#define SIMPLEPROTOCOL_H

/* SimpleProtocol
 * A simple protocol for communicating between devices
 * packets are constructed of 32 bit integers and validated through crc
 * Receiving is asynchronous and transmitting and synchronous
 */

/* 

The packet structure contains 32 bit ints


1) address
2) cmd
3) crc
4) up to 3 params
5) terminator


As some integers are reserved for special purposes, 
a range of -1B to 1B is allowed for all elements of the packets

*/

// all devices must have a 32 bit address, eg 0x000000001
#ifndef SIMPLEPROTOCOL_DEVICEADDRESS
#error "No SimpleProtocol address assigned to this device";
#endif 

#include <string.h>
#include "usart.h"
#include "queue.h"

#define SIMPLEPROTOCOL_DELIMITER 			0x2C		// ','
#define SIMPLEPROTOCOL_MAX_CMD_ELEMENT 		11			// 2^32 + 1 digit for '-'
#define SIMPLEPROTOCOL_MAX_PARAMS		 	3			// 2^32 + 1 digit for '-'
#define SIMPLEPROTOCOL_MAX_VAL				0x3B9ACA00	// 1B
#define SIMPLEPROTOCOL_ENDCMD				0x77359401	// 1B+1
#define SIPMLE_PROTOCOL_OVERFLOW			0x77359402	// 1B+2

// packet structure
typedef struct SimpleProtocolPacket{
	int32_t address;
	int32_t cmd;
	int32_t params[SIMPLEPROTOCOL_MAX_PARAMS];
	uint8_t cmd_count;
} SimpleProtocolPacket;


// parse a recieved string for commands and queue the results
void SimpleProtocol_parseRcv(char* rx);

// dequeue a full packet from the queue
SimpleProtocolPacket SimpleProtocol_getCmd();


#endif // #ifndef SIMPLEPROTOCOL_H
