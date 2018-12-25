#include "adc.h"
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t adc_inputs[] = { 0, 1, 2, 6, 7 }; //outputs to measure [+12.6, +8.4, +4.2, current_reference, current_value]
volatile uint16_t adc_reads[] = { 0, 0, 0, 0, 0 }; //for internal use

volatile uint16_t adc_window[5][10];

ISR(ADC_vect){
	read_next_adc();
}

void read_next_adc(){
	uint8_t i;
	uint32_t avg_data = 0;

	adc_reads[current_adc_input_idx] = ADCW;
	adc_window[current_adc_input_idx][window_pos] = adc_reads[current_adc_input_idx];
	
	for(i = 0; i < 10; i++){
		avg_data += adc_window[current_adc_input_idx][i];
	}
	adc_values[current_adc_input_idx] = avg_data / 10;


	current_adc_input_idx++;

	if(current_adc_input_idx > 4){
		current_adc_input_idx = 0;
		
		window_pos++;
		if(window_pos > 9){
			if(current_adc_input_idx == 0){
				adc_ready = 1;
			}
			window_pos = 0;
		}
	};

	ADMUX = (ADMUX & 0xF0) | adc_inputs[current_adc_input_idx];
}

void get_adc(){
	if(global_counter != last_processed_counter){
		ADCSRA |= 1 << ADSC;
		_delay_us(10);//to stabilize
	}
}

void process_adc(){
	//are we charging? 1 bit ~=50mA. We want to terminate at 68*7 = 476mA.
	//So we'll start at 13 (>~650mA) and end at 8 (<~400mA) (to have home hysteresis)
	if(adc_values[4] < adc_values[3] && (adc_values[3] - adc_values[4]) > 12){
		if(global_counter > 200){ //2s on startup delay before any blinking after start
			is_charging = 1;
		}
	} else {
		if(is_charging){
			//just to have some hysteresis
			if(adc_values[4] > adc_values[3]){
				is_charging = 0;
			}else{
				if((adc_values[3] - adc_values[4]) < 9){
					is_charging = 0;
				}	
			}
		}
		if(adc_values[4] > adc_values[3]){
			is_charging = 0;
		}
	}
}
