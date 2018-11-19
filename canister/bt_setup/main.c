#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "uart.h"

#define LDO_ON 			(PORTD |=  (1 << PD7))
#define LDO_OFF			(PORTD &= ~(1 << PD7))

#define BT_RESET_HIGH 	(PORTD |=  (1 << PD5))
#define BT_RESET_LOW	(PORTD &= ~(1 << PD5))

void reset_bt(){
	LDO_ON;
	BT_RESET_LOW;
	_delay_ms(100);
	BT_RESET_HIGH;
	_delay_ms(100);
}

void init(void){
	PORTB |= (1<<PINB1);	//turn on LED
	DDRB  |= (1<<PINB1);

	DDRD |= (1 << PD7); // LDO control output pin -> output	
	DDRD |= (1 << PD5); // BT reset pin -> output
}

static void reply_P(const char *addr){
    char c;
    while ((c = pgm_read_byte(addr++)))
    uart0_putc(c);
}

void send_config(){
	reply_P(PSTR("AT+DEFAULT\r\n"));
	_delay_ms(100);
	reply_P(PSTR("AT+NAMEBattery\r\n"));
	_delay_ms(100);
	reply_P(PSTR("AT+ROLE0\r\n"));
	_delay_ms(100);
	reply_P(PSTR("AT+PIN1234\r\n"));
	_delay_ms(100);
	reply_P(PSTR("AT+BAUD4\r\n"));
	_delay_ms(100);
}

int main(void){
	init();
	sei();

	uart0_init(UART_BAUD_SELECT(9600UL, F_CPU));
	
	reset_bt();
	send_config();

	uart0_init(UART_BAUD_SELECT(38400UL, F_CPU));
	reset_bt();
	send_config();
	reset_bt();

	for(;;){
		int c;
		c = uart0_getc(); //loopback - just to test
	    if ( !(c & UART_NO_DATA) ){
	    	uart0_putc(c);
		}
	}
	return 0;	
}