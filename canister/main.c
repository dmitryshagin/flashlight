#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "init.h"
#include "uart_proto.h"

//TOOD: reset global counter on power down;


// ISR(INT0_vect){
// 	cli();
// 	EIMSK &= ~(1<<0);
	
// 	_delay_ms(10);
// 	if(PIND & (1<<PIND2)){ //will ignore very short pulsed
// 		return;
// 	}
// 	OUT_INVERT;
// 	if( (PORTD & (1<<PD0)) ){ //Is output on?
// 		// BLUE_ON;
// 	}else{
// 		//output is off, turning everytnig off
// 		// RED_OFF;
// 		// GREEN_OFF;
// 		// BLUE_OFF;
// 	}
// 	while(!(PIND & (1<<PIND2))){_delay_ms(50); wdt_reset();} //wait for interrupt contidion to be reset + debounce
// 	_delay_ms(100); //we dont want to swith too often;
// }

// ISR(INT1_vect){
// 	cli();
// 	EIMSK &= ~(1<<1);
// 	// water_detected = 1;
// 	sei();
// }



int main(void){
	init();

	setLED(0xFF, 0xFF, 0xFF);
	_delay_ms(100);
	setLED(0, 0, 0);

	for(;;){
		process_uart();
		process_adc();
		//TODO: check water detection sensor;
		if(last_processed_counter != global_counter){
			last_processed_counter = global_counter;
		}
	};


	DDRC  = (1<<1);
	// (PORTB &= ~(1<<1)); //turn off adc
	(PORTC |=  (1<<1));
	// wdt_disable();

	// set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	// WTR_SENS_OFF;
	for(;;){


		// if( !(PINC & (1<<PC3)) ){ //Is output on?
		// 	BLUE_ON;
		// }else{
		// 	BLUE_OFF;			
		// }

		// if( (PIND & (1<<PD2)) ){ //Is input on?
		// 	GREEN_OFF;
		// }else{
		// 	GREEN_ON;
		// 	OUT_ON;
		// }

		// if( (PIND & (1<<PD3)) ){ //Is input getting external voltage (not only external)?
		// 	RED_OFF;
		// }else{
		// 	RED_ON;
		// 	OUT_ON;
		// }

		// if( (PORTC & (1<<PC2)) ){ //Is output on?
		// 	BLUE_ON;
		// }else{
		// 	BLUE_OFF;			
		// }
		// sleep_mode();
		// (PORTC |=  (1<<2));
		// _delay_ms(1000);
		// (PORTC &= ~(1<<2));
		// _delay_ms(1000);

	}

	// for(;;){
	// 	EIMSK |= (1<<0); //Re-Enable INT0 (will deisble every time when in interrupt)
	// 	sei();


	// 	if( (PORTC & (1<<PC0)) ){ //Is output on?
	// 		wdt_reset(); //? maybe reset WDT only if power is ON?
	// 		// RED_ON;
	// 		// GREEN_ON;
	// 		BLUE_ON;
	// 	}else{
	// 		wdt_reset();
	// 		//output is off, turning everytnig off
	// 		RED_OFF;
	// 		GREEN_OFF;
	// 		BLUE_OFF;
	// 		EIMSK |= (1<<0); //Re-Enable INT0 (will deisble every time when in interrupt)
	// 		sei();
	// 		// sleep_mode();
			
	// 	}
		
	// }
	return 0;	
}