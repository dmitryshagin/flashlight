#include "adc.h"
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t adc_inputs[] = { 0, 6, 7 }; //outputs to measure
volatile uint16_t adc_reads[] = { 0, 0, 0 }; //for internal use

volatile uint16_t voltage_window[] = { 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023 };
volatile uint16_t current_window[] = { 512,  512,  512,  512,  512,  512,  512,  512,  512,  512 };
volatile uint8_t window_pos = 0;
volatile uint8_t current_adc_input_idx = 0;

ISR(ADC_vect){
	adc_reads[current_adc_input_idx] = ADCW;
}

void get_adc(){
	uint8_t i;
	uint32_t avg_data = 0;
	if(global_counter != last_processed_counter){
		current_adc_input_idx++;

		if(current_adc_input_idx > 2){current_adc_input_idx = 0;};
		ADMUX = (ADMUX & 0xF0) | adc_inputs[current_adc_input_idx];
		
		//trigger switch to charge capacitor for vref & make window filtering
		if(current_adc_input_idx == 0){
			VREF_ON;
			voltage_window[window_pos] = adc_reads[0];
			window_pos++;
			if(window_pos > 9){
				window_pos = 0;
			}
			for(i = 0; i < 10; i++){
				avg_data += voltage_window[i];
			}
			adc_values[0] = avg_data / 10;
		}else{
			VREF_OFF;
			if (current_adc_input_idx == 1){
				current_window[window_pos] = adc_reads[1];
				for(i = 0; i < 10; i++){
					avg_data += current_window[i];
				}
				adc_values[1] = avg_data / 10;
			}else{
				adc_values[2] = adc_reads[2];
			}
		}
		ADCSRA |= 1 << ADSC;
		_delay_us(10);//to stabilize
	}
}

void process_adc(){
	if(global_counter < 25){
		return; //do nothing if we just woke up
	}
	
	if(adc_values[2] > adc_values[1] && (adc_values[2] - adc_values[1]) > 2){ //are we charging?
		is_charging = 1;
	} else {
		is_charging = 0;
	}
	//TODO: what should we do with our shiny ADC data? Switch led os stull like that?
}
