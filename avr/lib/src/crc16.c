#include "../crc16.h"


uint16_t crc16ccitt(char* s){	

	static uint16_t i;
	static uint16_t crc = 0xFFFF;
	for (i = 0; i < strlen(s); i++){
		crc = _crc_ccitt_update	(crc, s[i]);
	}
	return crc;
	
}

uint16_t crc16(char* s, uint8_t length){	

	uint16_t i;
	uint16_t crc;
	crc = 0xFFFF;
	for (i = 0; i < length; i++){
		crc = _crc16_update	(crc, s[i]);
	}
	return crc;
	
}
