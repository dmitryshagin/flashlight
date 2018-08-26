#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "init.h"
#include "adc.h"
#include "uart.h"
#include "uart_proto.h"

//910 - 12.230V
// 1 LSB ADC ~= 0,01343956044V
//10V - 744 ADC
//10.8V - 804 ADC
#define LOW_LIMIT 744
#define MEDIUM_LIMIT 804
#define HYSTERESIS 2

void process_LED(){
	if( is_on ){ //Is output on?
		if(is_leaking){
			int8_t blinker = (global_counter % 0x20) - 0xF;
			uint8_t val = 0;
			if(blinker < 0){
				val = 0xFF - blinker * 0xF;
			}else{
				val = blinker * 0xF;
			}
			set_LED(val,0,0);
		}else{
			if(is_charging){
				int8_t blinker = ((global_counter / 5) % 0x20) - 0xF;
				uint8_t val = 0;
				if(blinker < 0){
					val = 0xFF - blinker * 0xF;
				}else{
					val = blinker * 0xF;
				}
				set_LED(0,0,val/2); //not too bright
			}else{
				if(adc_values[0] < LOW_LIMIT){
					set_LED(0xFF,0,0);
				}else if(adc_values[0] > (LOW_LIMIT + HYSTERESIS) &&
				   		 adc_values[0] < MEDIUM_LIMIT){
					set_LED(0,0,0xFF);
				}else if(adc_values[0] > (MEDIUM_LIMIT + HYSTERESIS)){
					set_LED(0,0xFF,0);
				}

				//just started app and hit inside hysteresis value
				if(OCR1B == 0 && OCR1A == 0 && OCR0A == 0xFF){
					if(adc_values[0] < LOW_LIMIT){
						set_LED(0xFF,0,0);
					}else if(adc_values[0] < MEDIUM_LIMIT){
						set_LED(0,0,0xFF);
					}else{
						set_LED(0,0xFF,0);
					}					
				}
			}
		}
	}else{
		set_LED(0,0,0);
	}
}

void clear_int0_pin(){
	wdt_enable(WDTO_4S);
	while(!(PIND & (1<<PIND2))){_delay_ms(25);}
	reset_wdt();
	should_off = 0;
	should_on = 0;
	EIMSK |= (1<<0);
}

void process_state(){
	if(should_on){
		if(!is_on){
			turn_on();
			set_LED(0,0xFF,0);
			clear_int0_pin();
		}
	}

	if(should_off){
		if(is_on){
			set_LED(0,0,0);
			clear_int0_pin();
			turn_off();
		}
	}
	if(is_on){
		if(!(PINB & (1<<PB3))){ //Emergency condition - overcurrent!
			turn_off();
		}
	}
}

int main(void){
	init();

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	turn_off();

	for(;;){
		process_state();
		process_leakage();
		process_uart();
		get_adc();
		process_adc();
		process_temperature();
		if(adc_ready){
			process_LED();
		}
		
		if(last_processed_counter != global_counter){
			last_processed_counter = global_counter;
		}
	};
	return 0;	
}