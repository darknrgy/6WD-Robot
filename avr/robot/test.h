#include <string.h>

void simple_protocol_rx_st(char* s){
	
	char *next_token;
	char delim = ',';
	
	next_token = strtok(s, &delim);
	next_token = strtok(NULL, &delim);
	next_token = strtok(NULL, &delim);
}
