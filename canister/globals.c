#include "globals.h"

volatile uint16_t adc_values[] = { 0, 0, 0 };
volatile uint32_t global_counter = 0;
volatile uint32_t last_processed_counter = 0;
int16_t temp = 0;
uint8_t leakage = 0;
