#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <avr/io.h>

extern volatile uint16_t adc_values[];
extern volatile uint32_t global_counter, last_processed_counter;

#endif
