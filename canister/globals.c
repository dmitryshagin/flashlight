#include "globals.h"

volatile uint16_t adc_values[] = { 0, 0, 0 }; //voltage, current, 2.5V reference
volatile uint32_t global_counter = 0;
volatile uint32_t last_processed_counter = 0;
volatile int16_t temp = 0;
volatile uint8_t leakage = 0;
volatile uint8_t is_charging = 0;
volatile uint8_t is_on = 0;
