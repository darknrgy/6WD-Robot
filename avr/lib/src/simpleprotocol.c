#include "../simpleprotocol.h"

// retrieve a queue structure
static Queue* SimpleProtocol_getQueue();


void SimpleProtocol_parseRcv(char* buffer){
	
	static char next_token_buffer[SIMPLEPROTOCOL_MAX_CMD_ELEMENT];
	static char* next_token;
	static int32_t next_command_element;
	static char delim = SIMPLEPROTOCOL_DELIMITER;
	static Queue* queue;

	queue = SimpleProtocol_getQueue();
	
	// tokenize the buffer and queue into the Queue
	next_token = strtok(buffer, &delim);
	while (next_token != 0x0000){
		strcpy(next_token_buffer, next_token);
		
		// enqueue this command element into the Queue
		next_command_element = atol(next_token_buffer);
		Queue_enqueue(queue, next_command_element);


		next_token = strtok(NULL, &delim);
	}


	int32_t testint; 
	while ( (testint = Queue_dequeue(queue)) ){
		
	}


	return;
}

SimpleProtocolPacket SimpleProtocol_getCmd(){
	
	static Queue* queue;
	static SimpleProtocolPacket packet;
	static int32_t element;
	static int32_t crc;
	static uint8_t i = 0;
	static uint8_t queue_error;
	
	queue = SimpleProtocol_getQueue();	
	
	// build a packet 
	packet.cmd_count = 0;
	while ((element = Queue_dequeue(queue)) != SIMPLEPROTOCOL_ENDCMD 
			&& i < (3 + SIMPLEPROTOCOL_MAX_PARAMS)){

		if (( queue_error = Queue_geteError(queue))){
			// throw error about the queue
			
		}
		i++;
		switch (i){
			case 1:
				packet.cmd = element; break;
			case 2: 
				crc = element; break;
			default:
				packet.params[i-3] = element; break;
		
		}
	}

	if (i < 3){
		// throw error indicating that the packet is incomplete
	}

	// calculate crc and check value, return error if broken

	packet.cmd_count = i - 2;

	return packet;

}

static Queue* SimpleProtocol_getQueue(){
	static Queue queue;
	static char is_created = 0x00;
	if (is_created == 0x00){
		// create the queue
		queue = Queue_create(20);
		is_created = 0x01;
	}
	return &queue;
}

void usart_rx_callback(char *buffer){

	cli();
	SimpleProtocol_parseRcv(buffer);
	sei();

}



/*
void cunt(char *asdasd){
	//static char next_token[SIMPLEPROTOCOL_MAX_CMD_ELEMENT];
	//static char *iterator;
	//static char next_token[4];
	//iterator = rx;
	//while (iterator[0] != NULL){
		//strncpy(next_token, iterator, 4);
		//iterator++;
	//}
}

void usart_rx_callback(char *s){

	cli();
	//simple_protocol_rx_str(s);
	sei();

}


void fuckyoutest(char* s){

	char *next_token;
	char *tokens[5];
	char delim = ',';
	
	tokens[0] = next_token = strtok(s, &delim);
	tokens[0] = next_token = strtok(NULL, &delim);
	tokens[0] = next_token = strtok(NULL, &delim);

	while (next_token != NULL && count < 5){
		if (strlen(next_token) > 0){
			count++;
			tokens[count] = next_token = strtok(NULL, &delim);
		}			
		
	}

	

	for (int i = 0; i < (count); i ++){
		usart_psend(PSTR("A token is: "));		
		usart_print_int32_hex(atol(tokens[i]));	
		//usart_send(tokens[i]);
		usart_psend(PSTR("\n"));	

	}
	
	
}
*/

