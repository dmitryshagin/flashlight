#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 

int delay(){
	uint16_t i=0;
	for(i=0;i<10000;i++){
		asm volatile("nop");
	}	
}

ISR(INT0_vect){
	cli();
	PORTB^=_BV(PORTB1);
	delay();
	while(!(PINB & (1<<PINB2))){}
	delay();
	sei();
}

int main(void){
	DDRB  = (1<<DDB1) | (0<<DDB0) | (0<<DDB2) | (0<<DDB3);
	PUEB  = (1<<PORTB2);
	PORTB = (1<<PORTB3) | (1<<PORTB2) | (0<<PORTB1) | (1<<PORTB0);
	EIMSK = (1<<INT0);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
	while(1){
		sleep_mode();
	}
}