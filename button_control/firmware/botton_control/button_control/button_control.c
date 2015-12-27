//SET fuses to 4MHz + CKDIV8 (0xFF/0x69)
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 

ISR(INT0_vect){
	cli();
	GIMSK=0;
	PORTB^=_BV(PORTB4);
	for(uint16_t i=0;i<6000;i++){
		asm volatile("nop");
	}
	while(!(PINB & (1<<PINB1))){}
	GIMSK=(1<<INT0);
	sei();
}

int main(void){
	DDRB=(0<<DDB5) | (1<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
	PORTB=(1<<PORTB5) | (0<<PORTB4) | (1<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);
	GIMSK=(1<<INT0);
	MCUCR=0;
	GIFR=(1<<INTF0);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
	while(1){
		sleep_mode();
	}
}