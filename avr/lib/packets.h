#ifndef PACKETS_H
#define PACKETS_H

/*
#ifndef PACKETS_DEVICEADDRESS
#error "No SimpleProtocol address assigned to this device";
#endif 
*/


#include <string.h>
#include <util/crc16.h>
#include "usart.h"
#include "queue.h"

#define PACKETS_MAXLENGTH 		8 // 8 bytes
#define PACKETS_QUEUESIZE		8 
#define PACKETS_ERRORCMD		0xFF
#define PACKETS_TERMINATOR_LEN	5
#define PACKETS_SEND_ADD		0xFA

// Packets Errors
#define PACKETS_ERR_CRC			1

// packet structure
typedef struct Packet{
	uint8_t address;
	char data[PACKETS_MAXLENGTH];
	uint16_t crc;
	uint8_t length;
} Packet;

typedef uint8_t PacketsError;


// retrieve a queue structure
Queue* Packets_getQueue();

// dequeue a full packet from the queue
Packet Packets_getNext();

// parse a recieved string for packets
void Packets_rxCallback(char* rx, uint8_t);

// throw  an error through Packets
void Packets_sendError(uint8_t feature, uint8_t error);

// send a packet
void Packets_send(Packet packet);


// check and clear error
uint8_t Packets_getError();

#endif // #ifndef PACKETS_H
