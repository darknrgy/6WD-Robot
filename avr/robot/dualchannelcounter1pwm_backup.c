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

	// initialize pwm
	pwm_init();
	pwm_set_scalar(1);
	pwm_set(PWM_CHAN_A, 127);

	// initialize Time
	time_init();
	TimeResult tr, rpm_tr;
	uint32_t previous = 0;
	rpm_tr.previous = 0;

	// enable external interrupt for INT0
	//setbit(MCUCR, BIT(ISC01) | BIT(ISC00));
	setbit(MCUCR,BIT(ISC00));
	setbit(GICR, BIT(INT0));

	// initialize rpm counter
	InterruptCounterResult icr; 
	icr.previous = 0;
	float rpm = 0, prev_rpm = 0, time_ms = 0, rpm_delta = 0;

	uint16_t* rpm_target_p;
	float pwm_power = 0;
	float pwm_acc = 0;
	float rpm_target = 0;
	
	rpm_target_p = get_rpm_handle();
	*rpm_target_p =  1000;
	

	// initialize usart
	usart_init();

	// enable interrupts
	sei();

	// set initial value
	pwm_set_scalar(3);
	pwm_set(1,256);

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
			// calculate RPM
			rpm_tr = time_get_time_delta(rpm_tr.previous);
			icr = interrupt_counter_delta(icr.previous);
			time_ms = get_time_in_ms(rpm_tr.delta);
			rpm =  (double) icr.delta /(double) 32 / (double) time_ms * (double) 1000 * (double) 60;
			
			rpm_target_p = get_rpm_handle();
			rpm_target  = (float) *rpm_target_p;
			rpm_delta = rpm_delta * 0.65 + (rpm - prev_rpm) * 0.35;
			
			
			//pwm_acc = (rpm_target - rpm) * 0.01 - rpm_delta * 0.05;
			pwm_acc = (rpm_target - rpm) * 0.015 - rpm_delta * 0.15;
			
			if (pwm_acc < - pwm_power) pwm_power = 0;
			else if (pwm_acc + pwm_power > 1023) pwm_power = 1023;
			else{
				pwm_power = pwm_power + pwm_acc;
			}

			
			if (rpm_target == 0) pwm_power = 0;
			pwm_set(1,floor(pwm_power));
			prev_rpm = rpm;

			cmd_send_debug16( 'r', (uint16_t) rpm);

			

		}	
	}

	
}

/*
ISR(INT0_vect, ISR_NOBLOCK){
	
	uint16_t* counter = interrupt_counter_1();
    *counter = *counter + 1;
   

}*/

uint16_t* interrupt_counter_1(){
	static uint16_t counter;
	return &counter;

}


InterruptCounterResult interrupt_counter_delta(uint16_t previous){

    InterruptCounterResult result;
    uint16_t* current = interrupt_counter_1();
    if (*current >= previous) result.delta = *current - previous;
    else result.delta = (uint16_t) INTERRUPT_COUNTER_MAX - previous + *current;
    result.previous = *current;
    return result;
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


