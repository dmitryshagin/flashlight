#ifndef __ADC_H__
#define __ADC_H__

#include <avr/io.h>
#include "init.h"

void read_next_adc();
void get_adc();

void process_adc();

#endif	// _ADC_H_
