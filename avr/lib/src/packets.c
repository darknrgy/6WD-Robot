#include "../packets.h"
#include "../crc16.h"

// get a pointer to the error 
static uint8_t* Packets_error();

// set an error
static void Packets_setError(uint8_t value);

// parse a recieved byte stream for packets
void Packets_rxCallback(char* rx, uint8_t length){
	
	length -= PACKETS_TERMINATOR_LEN;
	uint8_t crc_high = length-2;
	uint8_t crc_low = length-1;
	
	Packet packet;
	packet.crc = 0;
	packet.length = length-3;

	for (uint8_t i = 0; i < length; i++){
		if (i == 0) packet.address = rx[i]; 
		else if (i == crc_high){
			packet.crc = rx[i]; 
			packet.crc = packet.crc << 8 ; 
		}
		else if (i == crc_low) packet.crc += rx[i];
		else packet.data[i-1] = rx[i]; 
	}

		
	// check crc
	uint16_t crc = crc16(rx, length-2);
	if (crc != packet.crc){
		Packets_setError(PACKETS_ERR_CRC);
		return;
	}
	
	

	Queue* queue; 
	queue = Packets_getQueue();
	Queue_enqueue(queue, &packet);

}

Packet Packets_getNext(){
	
	static Queue* queue; queue = Packets_getQueue();
	Packet packet;
	Queue_dequeue(queue, &packet);
	return packet;
}

void Packets_sendError(uint8_t feature, uint8_t error){

	// send an error back over the stream
	Packet packet;
	packet.data[0] = PACKETS_ERRORCMD;
	packet.data[1] = feature;
	packet.data[2] = error;
	packet.length = 3;
	Packets_send(packet);
}

void Packets_send(Packet packet){

	char buffer[PACKETS_MAXLENGTH+8];
	//char terminator[PACKETS_TERMINATOR_LEN] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	buffer[0] = PACKETS_SEND_ADD;
	memcpy(&buffer[1], &packet.data[0], packet.length);
	uint16_t crc = crc16(buffer, packet.length+1);
	buffer[packet.length+1] = crc >> 8;
	buffer[packet.length+2] = crc & 0x00FF;
	//memcpy(&buffer[packet.length+3], &terminator, PACKETS_TERMINATOR_LEN);
	usart_send_by_length(buffer, packet.length+3);
}

void usart_rx_callback(char *buffer, uint8_t length){

	cli();
	Packets_rxCallback(buffer, length);
	sei();

}



Queue* Packets_getQueue(){
	static Queue queue;
	static char is_created = 0x00;
	if (is_created == 0x00){
		// create the queue
		queue = Queue_create(10, sizeof (Packet) );
		is_created = 0x01;
	}
	return &queue;
}

static uint8_t* Packets_error(){
	static uint8_t error = 0;	
	return &error;
}

uint8_t Packets_getError(){
	uint8_t* error = Packets_error();
	uint8_t value = *error;
	*error = 0x00;
	return value;
}

static void Packets_setError(uint8_t value){
	uint8_t* error = Packets_error();
	*error = value;
}


