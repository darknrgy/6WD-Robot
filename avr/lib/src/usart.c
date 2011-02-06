#include "../usart.h"


char usart_rxbuff[32];
uint8_t usart_position;

void usart_init(){
    
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    #if USE_2X
    setbit(UCSRA, BIT(U2X));
    #else
    clearbit(UCSRA, BIT(U2X));
    #endif

	// Enable receiver and transmitter
	UCSRB |= _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
}

void usart_putchr(char c){
	RETURN_IF_AVRSIM;
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
}

void usart_send(const char *s){
	RETURN_IF_AVRSIM;
	while (*s){
		if (*s == '\n') usart_putchr('\r');
      	usart_putchr(*s++);
    }
}

void usart_send_by_length(const char *s, uint8_t length){
	RETURN_IF_AVRSIM;
	static char terminator[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	for (uint8_t i = 0; i < length; i++){
		usart_putchr(s[i]);
	}
	for (uint8_t i = 0; i < 5; i ++){
		usart_putchr(terminator[i]);
	}
}

void usart_psend(const char *s){
	RETURN_IF_AVRSIM;
    static char terminator[] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	char c;
    for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s)){
        usart_putchr(c);
    }
	for (uint8_t i = 0; i < 5; i ++){
		usart_putchr(terminator[i])	;	
	}


}


ISR(USART_RXC_vect){
    
    if (!bit_is_clear(UCSRA, FE)){
 		usart_position = 0;
		usart_psend(PSTR("!framing_error\n"));
        return;
    }

	uint8_t c;
	c = UDR;
	usart_parse_rx(c);
}

void usart_parse_rx(uint8_t c){
	
	// detect packet trailer 0x00 0xFF 0xFF 0xFF 0xFF
	// small finite state machine here
	static uint8_t trailer = 0;

	if (c == 0x00) trailer = 1;
	else if (c == 0xFF && trailer > 0) trailer ++;
	else trailer = 0;   
	
	if (trailer > 4){
		trailer = 0;
		usart_rx_callback(usart_rxbuff, usart_position+1);
		usart_position = 0;
    	return;
	}

	usart_position ++;
    
    if (usart_position >= 64){
        usart_psend(PSTR("!buffer_full\n"));
    }
    
    usart_rxbuff[usart_position-1] = c;		
	
}



void usart_print_int32(uint32_t v){
	RETURN_IF_AVRSIM;
	usart_putchr('>');
	usart_putchr('>');
	char str[64] = "";
	sprintf(str, "%" PRIi32, v);
	usart_send_by_length(str, strlen(str));	
	
}


void usart_print_int32_hex(uint32_t v){
	RETURN_IF_AVRSIM;
	char str[64] = "";
	sprintf(str, "%" PRIX32, v);
	usart_send_by_length(str, strlen(str));	
	
}


