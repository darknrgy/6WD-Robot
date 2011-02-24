#ifndef COMMANDS_H
#define COMMANDS_H

// packet handler
void handle_packet(Packet packet);

// commands (recv)
void cmd_pwm_set(char* params);
void cmd_pwm_setscalar(char* params);
void cmd_rpmset(char* params);

// commands (send)
void cmd_send_rpm(uint16_t rpm);
void cmd_get_rpm();
void cmd_send_debug16(char name, uint16_t debug);
void cmd_sendstatus(char id, uint16_t v1, uint16_t v2, uint16_t v3);


// utility
uint16_t* get_rpm_handle();

#endif // #define COMMANDS_H
