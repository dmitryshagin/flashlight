#ifndef __INIT_H__
#define __INIT_H__

#include "globals.h"
#include <avr/io.h>
#include <avr/wdt.h>
// #include "adc.h"
#include "include/ds18b20/ds18b20.h"

// Pinout:
// 1< PD3 - INT1 - Signal from external power on (active low, pulled up)
// 2> PD4 - BT_KEY - pull up to switch BT to CMD mode for HC-05. No effect for HC-06 or similar
// 3  GND
// 4  +3.3
// 5  GND
// 6  +3.3
// 7  PB6 - XTAL1 (3.6864Mhz)
// 8  PB7 - XTAL2 (3.6864Mhz)
// 9> PD5 - BT_RESET - reset if low for 5ms
// 10> PD6 - OC0A - GREEN LED
// 11> PD7 - LDO Shutdown (tie to GND for shutdown)
// 12<>PB0 - 1-Wire. DS18B20
// 13> PB1 - OC1A - BLUE LED
// 14> PB2 - OC1B - RED LED
// 15< PB3 - MOSI / input from overcurrent fault. No external pullup
// 16> PB4 - MISO / current measure output
// 17> PB5 - SCK / output - turn on output and voltage sensors
// 18  AVCC - 3.3V
// 19< ADC6 - Current measure reference
// 20  AREF - GND
// 21  GND
// 22< ADC7 - Current measure input
// 23< PC0 - Voltage measure input (12.6V)
// 24< PC1 - 8.4VIN
// 25< PC2 - 4.2VIN
// 26< PC3 - Leakage input (low => leakage present, pulled up externally)
// 27<>PC4 - SDA
// 28> PC5 - SCL
// 29< PC6 - RESET
// 30< PD0 - RX
// 31> PD1 - TX
// 32< PD2 - INT0 - button input (active low). No external pullup

#define TIMER_TUNE 		(0xFF - 144) // to get 100 Hz overflow interrrupts


#define LDO_ON 			(PORTD |=  (1 << PD7))
#define LDO_OFF			(PORTD &= ~(1 << PD7))


#define OUT_ON 			(PORTB |=  (1 << PB5))
#define OUT_OFF			(PORTB &= ~(1 << PB5))

#define MEASURE_ON  	(PORTB &= ~(1 << PB4))
#define MEASURE_OFF		(PORTB |=  (1 << PB4))

#define BT_RESET_HIGH 	(PORTD |=  (1 << PD5))
#define BT_RESET_LOW	(PORTD &= ~(1 << PD5))



void init();
void reset_wdt();

void set_LED(uint8_t R, uint8_t G, uint8_t B);

void turn_on();
void turn_off();

void process_temperature();
void process_leakage();

#endif	// _INIT_H_
