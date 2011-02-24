#include "includes.h"
#include "constants.h"
#include "commands.h"


void handle_packet(Packet packet){

	char *params = &packet.data[1];
	
	switch (packet.data[0]){
		case CMD_PWM_SET:			cmd_pwm_set(params); break;
		case CMD_PWM_SETSCALAR:		cmd_pwm_setscalar(params); break;
		case CMD_RPMSET		:		cmd_rpmset(params); break;
		default: 					Packets_sendError(ERROR_INVALIDCMD, 0x00);			

	}
	
}

void cmd_pwm_set(char* params){
	char chan = params[0];
	uint16_t value = params[1] << 8; value += params[2];	
	pwm_set(chan, value);

}

void cmd_pwm_setscalar(char* params){
	pwm_set_scalar(params[0]);
}

void cmd_rpmset(char* params){

	
	int16_t left = params[0] << 8; left += params[1];
	int16_t right = params[2] << 8; right += params[3];

	left  -= 10000;
	right -= 10000;

	motors_set(MOTORS_LEFT, left);
	motors_set(MOTORS_RIGHT, right);
}

uint16_t* get_rpm_handle(){
	static uint16_t rpm;
	return &rpm;
}


void cmd_send_rpm(uint16_t rpm){
	
	Packet packet;
	packet.data[0] = CMD_RPM;
	packet.data[1] = rpm >> 8;
	packet.data[2] = rpm - (packet.data[1] << 8);
	packet.length = 3;
	Packets_send(packet);
}

void cmd_send_debug16(char name, uint16_t debug){

	Packet packet;
	packet.data[0] = CMD_DEBUG16;
	packet.data[1] = name;
	packet.data[2] = debug >> 8;
	packet.data[3] = debug - (packet.data[1] << 8);
	packet.length = 4;
	Packets_send(packet);

}

void cmd_sendstatus(char id, uint16_t v1, uint16_t v2, uint16_t v3){
	Packet packet;
	packet.data[0] = CMD_SENDSTATUS;
	packet.data[1] = id;
	packet.data[2] = v1 >> 8;
	packet.data[3] = v1 - (packet.data[2] << 8);
	packet.data[4] = v2 >> 8;
	packet.data[5] = v2 - (packet.data[4] << 8);
	packet.data[6] = v3 >> 8;
	packet.data[7] = v3 - (packet.data[6] << 8);	
	packet.length = 8;
	Packets_send(packet);

}

void cmd_get_rpm(){



}
