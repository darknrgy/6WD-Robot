/* Test Application of Dual Channel Counter1 PWM
 */

#include "constants.h"
#include "includes.h"

// External Interrupt Setup
#define INTERRUPT_COUNTER_MAX 65535
typedef struct InterruptCounterResult{
	uint16_t delta;
	uint16_t previous;
} InterruptCounterResult;
uint16_t* interrupt_counter_1();
InterruptCounterResult interrupt_counter_delta(uint16_t previous);




int main( void ){

	uint8_t error;
	Packet packet;
	Queue* packets;
	packets = Packets_getQueue();

	// initialize Time
	time_init();
	TimeResult tr;
	uint32_t previous = 0;

	// initialize usart
	usart_init();

	// initialize motors
	motors_init();
	motors_set(MOTORS_LEFT , MOTORS_FWD, 2444);
	motors_set(MOTORS_RIGHT, MOTORS_FWD, 2444);

	Motor* motor1 = motors_get_motor(MOTORS_LEFT);
	Motor* motor2 = motors_get_motor(MOTORS_RIGHT);

	// enable interrupts
	sei();

	float diff = 0;

	// loop forever
	while(1){
		error = Packets_getError();
		if (error){
			Packets_sendError(ERROR_PACKETS, error);
			continue;
		}
		if (packets->count > 0){
			error = 0x00;
			packet = Packets_getNext();
			error = Queue_getError(packets);
			if (error){
				Packets_sendError(ERROR_QUEUE, error);
				continue;
			}

			handle_packet(packet);
		}
		tr = time_get_time_delta(previous);
		
		// every 20ms, calculate rpm
		if (get_time_in_ms(tr.delta) > 20){
			previous = tr.previous;
			motors_tick();
			
			diff = diff * 0.00 + ((uint16_t) motor1->pwm - motor2->pwm) * 1.00;

			cmd_send_debug16( 'd', (uint16_t) (abs(diff)) );

			

		}	
	}
	
}






/*
int rand_range(int a, int b){
	int t = rand();
	t = t % (b-a+1);
	t += a;
	return t;
}*/

/*

	
	Packet packet;
	packet.data[0] = 0x02;
	packet.data[1] = 0x03;
	packet.data[2] = 0x04;
	packet.data[3] = 0x05;
	packet.data[4] = 0x06;
	packet.length = 5;

	Packets_send(packet);
	
		
	char data[] = 
	//{'2','8','9','7','a','h','a','a', 0xF0, 0xFD, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	//{0x01,0x00,0x56,0x9A,0xAE, 0xCC, 0x412, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	//{0x01,0x00,0x56,0x9A,0xAE};
	{0x01,0x01,0x02,0x03,0x79,0x11, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	
	for (uint8_t i = 0; i < sizeof(data); i++){
		usart_parse_rx(data[i]);
	}
	

	for (uint8_t i = 0; i < sizeof(data); i++){
		usart_parse_rx(data[i]);
	}

*/


/*
	uint8_t error;
	Packet packet;
	Queue* packets;
	packets = Packets_getQueue();
		
	Queue_dequeue(packets, &packet);
	error = Queue_getError(packets);
	uint8_t a = 1;
	*/
	/*
	Packets packet;
	packet.address = 0xFF;
	packet.data = data;
	packet.crc = crc;
	*/






	
	/*
	
	// initialize usart
	usart_init();

	// enable interrupts
	sei();

	char test_rx[] = {0,1,2,3,4,5,6,7,8,0xFF,0xFF}
	char *test_rx_p = test_rx;
	SimpleProtocol_parseRcv(test_rx_p);

	*/






	/*
	Queue packets;
	packets = Queue_create(5, sizeof (Packet));
	char data[] = "12345678";
	uint8_t error;
	int i;

	Packet packet;
	Packet outpacket;

	packet.address = 0xFF;
	strcpy(packet.data, data);
	packet.crc = 0xFF;

	for (i = 0; i < 1; i ++){
		
		packet.address = i;
		Queue_enqueue(&packets, &packet);
		error = Queue_getError(&packets);
	}
		

	for (i = 0; i < 1; i ++){
		// a = * (int *) r;
		Queue_dequeue(&packets, &outpacket);
		error = Queue_getError(&packets);
	}
	
	int a = 5;
	*/

	

	/*

	for(;;){
        sleep_mode();
    }
*/


