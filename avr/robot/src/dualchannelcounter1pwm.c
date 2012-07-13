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


void send_status();

int main( void ){


	DDRC = 0xDB;
	//PORTC = 0x7E;;
	PORTC = 0xE7;


	uint8_t error;
	Packet packet;
	Queue* packets;
	packets = Packets_getQueue();

	// initialize globals 
	globals_init();

	// initialize Time
	time_init();
	TimeResult tr;
	uint32_t previous = 0;
	uint32_t watchdog = 0;

	// initialize usart
	usart_init();

	// initialize motors
	motors_init();
	motors_set(MOTORS_LEFT, 0);
	motors_set(MOTORS_RIGHT, 0);

	// initialize analog input
	analoginput_init();

	// enable interrupts
	sei();

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
			watchdog = time_get_time();
			
		}
		tr = time_get_time_delta(previous);
		
		// every 20ms, calculate rpm
		if (get_time_in_ms(tr.delta) > 20){
			previous = tr.previous;
			motors_tick();
			send_status();

		}	

		tr = time_get_time_delta(watchdog);
		if (get_time_in_ms(tr.delta) > 500){
			motors_set(MOTORS_LEFT, 0);
			motors_set(MOTORS_RIGHT, 0);
		}	
	}
	
}

void send_status(){

		static float battery_voltage, left_motor_amps, right_motor_amps;
		static uint8_t scalar = 0;
		

		Motor* motorL;
		Motor* motorR;
		motorL = motors_get_motor(MOTORS_LEFT);
		motorR = motors_get_motor(MOTORS_RIGHT);


		battery_voltage = battery_voltage * 0.90 + (analoginput_get(BATTERY_VOLTAGE) * (5/1024.0f) * 1000 * 5.54) * 0.1;

		left_motor_amps = left_motor_amps  * 0.98f + ( (float) analoginput_get(LEFT_MOTOR_AMPS) /120.8 ) * 0.02f;
		right_motor_amps = right_motor_amps  * 0.98f + ( (float) analoginput_get(RIGHT_MOTOR_AMPS) /120.8 ) * 0.02f;

		scalar ++;
		if (scalar > 50) scalar = 0;
		
		if (scalar == 10) cmd_sendstatus('L', (uint16_t) abs(motorL->rpm_measured),  (uint16_t) (left_motor_amps * battery_voltage),  (uint16_t) abs(motorL->pwm));
		if (scalar == 20) cmd_sendstatus('R', (uint16_t) abs(motorR->rpm_measured),  (uint16_t) (right_motor_amps * battery_voltage),  (uint16_t) abs(motorR->pwm));
		if (scalar == 30) cmd_sendstatus('M', (uint16_t) battery_voltage, 0,0);
		
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


