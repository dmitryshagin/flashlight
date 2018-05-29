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
				// set_LED(0,val,0);
				//TODO - fine-tune voltages
				// if(adc_values[0] < 800){
					set_LED(0,0,val);
				// }else{
					// set_LED(0,val,0);
				// }
			}else{
				//TODO: ugly hack. need to calculate proper value for ADC limits.
				//TODO; need some hysteresis to prevent multible swithing on borders
				//now 934 = 12.6V
				//let's limit at 10 & 10.8V;
				if(adc_values[0] < 740){
					set_LED(0xFF,0,0);
				}else if(adc_values[0] < 800){
					set_LED(0,0,0xFF);
				}else{
					set_LED(0,0xFF,0);
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
		
		// char _tmpstr[UART_RX0_BUFFER_SIZE];
		if(last_processed_counter != global_counter){
			// if(adc_ready){
				// sprintf(_tmpstr, "%04d\n\r", adc_values[0]);
				// uart0_puts(_tmpstr);
			// }

			last_processed_counter = global_counter;
		}
	};
	return 0;	
}