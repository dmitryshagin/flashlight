#ifndef __INIT_H__
#define __INIT_H__

#include "globals.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include "adc.h"
#include "include/ds18b20/ds18b20.h"


// Pinout:
// 1 PD3 - INT1 - Signal from external power on (active low)
// 2 PD4 - BT_KEY - pull up to switch BT to CMD mode for HC-05. No effect for HC-06 or similar
// 3 GND
// 4 +3.3
// 5 GND
// 6 +3.3
// 7 PB6 - XTAL1 (3.6864Mhz)
// 8 PB7 - XTAL2 (3.6864Mhz)
// 9 PD5 - BT_RESET - reset if low for 5ms
// 10 PD6 - OC0A - GREEN LED
// 11 PD7 - LDO Shutdown (tie to GND for shutdown)
// 12 PB0 - 1-Wire. DS18B20
// 13 PB1 - OC1A - BLUE LED
// 14 PB2 - OC1B - RED LED
// 15 PB3 - MOSI
// 16 PB4 - MISO
// 17 PB5 - SCK
// 18 AVCC - 3.3V
// 19 ADC6 - Current measure input
// 20 AREF - GND
// 21 GND
// 22 ADC7 - Current measure reference
// 23 PC0 - Voltage measure input
// 24 PC1 - Voltage measure transistor output (via 100n capacitor). Switch from high to low to measure
// 25 PC2 - Load output (active high)
// 26 PC3 - Leakage input (low => leakage present, pulled up externally)
// 27 PC4 - Leagake sensor turn on (active high)
// 28 PC5 - Current sensor turn on (active low)
// 29 PC6 - RESET
// 30 PD0 - RX
// 31 PD1 - TX
// 32 PD2 - INT0 - button input (active low)

#define TIMER_TUNE 		(0xFF - 144) // to get 100 Hz overflow interrrupts

#define OUT_ON 			(PORTC |=  (1 << PC2))
#define OUT_OFF 		(PORTC &= ~(1 << PC2))
#define OUT_INVERT 		(PORTC ^=  (1 << PC2))

#define WTR_SENS_ON 	(PORTC |=  (1 << PC4))
#define WTR_SENS_OFF	(PORTC &= ~(1 << PC4))

#define VREF_OFF 		(PORTC |=  (1 << PC1))
#define VREF_ON 		(PORTC &= ~(1 << PC1))

#define BT_CMD_ON 		(PORTD |=  (1 << PD4))
#define BT_CMD_OFF		(PORTD &= ~(1 << PD4))

#define BT_RESET_OFF 	(PORTD |=  (1 << PD5))
#define BT_RESET		(PORTD &= ~(1 << PD5))

#define BT_PWR_ON 		(PORTD |=  (1 << PD7))
#define BT_PWR_OFF		(PORTD &= ~(1 << PD7))



void init();
void reset_wdt();

void setLED(uint8_t R, uint8_t G, uint8_t B);

void turn_everything_on();
void turn_everything_off();

void processTemp();
void processLeakage();

#endif	// _INIT_H_
