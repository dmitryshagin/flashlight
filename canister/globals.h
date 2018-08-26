#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <avr/io.h>

extern volatile uint16_t adc_values[5];
extern volatile uint32_t global_counter, last_processed_counter;
extern volatile int16_t temperature; //stores temperature * 16

extern volatile uint8_t is_leaking;
extern volatile uint8_t is_charging;
extern volatile uint8_t is_on;
extern volatile uint8_t should_on;
extern volatile uint8_t should_off;
extern volatile uint8_t adc_ready;
extern volatile uint8_t window_pos;
extern volatile uint8_t current_adc_input_idx;


#endif
