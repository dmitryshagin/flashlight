#include "globals.h"

volatile uint16_t adc_values[] = { 0, 0, 0, 0, 0 }; //+12V, +8.4V, +4.2V, current, 2.5V reference
volatile uint32_t global_counter = 0;
volatile uint32_t last_processed_counter = 0;
volatile int16_t temperature = 0;
volatile uint8_t is_leaking = 0;
volatile uint8_t is_charging = 0;
volatile uint8_t is_on = 0;
volatile uint8_t should_on = 0;
volatile uint8_t should_off = 0;
volatile uint8_t adc_ready = 0;
volatile uint8_t window_pos = 0;
volatile uint8_t current_adc_input_idx = 0;
volatile uint16_t fram_position = 0;
