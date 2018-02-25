#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "init.h"
#include "uart_proto.h"

void go_to_sleep(){
	last_processed_counter = 0;
	global_counter = 1;
	turn_everything_off();
	EIMSK |= (1<<0);
	EIMSK |= (1<<1);
	sleep_mode();
}


void processLED(){
	if( (PINC & (1<<PC2)) ){ //Is output on?
		if(leakage){
			int8_t blinker = (global_counter % 0x20) - 0xF;
			uint8_t val = 0;
			if(blinker < 0){
				val = 0xFF - blinker * 0xF;
			}else{
				val = blinker * 0xF;
			}
			setLED(val,0,0);
		}else{
			//TODO - led by voltage
			setLED(0,0,0xFF);
		}
	}else{
		setLED(0,0,0);
	}
}


void processState(){
	processLED();
	if( (PINC & (1<<PC2)) ){ //Is output on?
		// we''l have overlapping interrupts if we react on both int0 and int1
		// EIMSK &= ~(1<<1);
		//TODO - check voltage
		
	}else{
		go_to_sleep();
	}
}

int main(void){
	init();

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	OUT_OFF;

	for(;;){
		process_uart();
		get_adc();
		process_adc();
		processTemp();
		processLeakage();
		processState();
		EIMSK |= (1<<0);

		if(last_processed_counter != global_counter){
			last_processed_counter = global_counter;
		}
	};
	return 0;	
}