#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <avr/io.h>

extern volatile uint16_t adc_values[];
extern volatile uint32_t global_counter, last_processed_counter;
extern volatile int16_t temp; //stores temperature * 16

extern volatile uint8_t leakage;
extern volatile uint8_t is_charging;
extern volatile uint8_t is_on;

#endif
