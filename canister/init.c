#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include "init.h"
#include "uart.h"
#include "ds18b20.h"

ISR(TIMER2_OVF_vect){ // ~100Hz
	TCNT2 = TIMER_TUNE;
	global_counter++;
}


void setLED(uint8_t R, uint8_t G, uint8_t B){
	OCR1B = R;
	OCR0A = 0xFF - G; // For unknown reason this PWM channer works better in inverse mode.
	OCR1A = B;
}

void init_LED(){
	//PWM Pins as Out
	DDRB |= (1 << PB1) | (1 << PB2);
	DDRD |= (1 << PD6);

	TCCR0A |= (1 << COM0A1) | (1 << COM0A0) | (1 << WGM00 | 1 << WGM01);
	TCCR0B |= (1 << CS00) | (1 << CS01); //x64 prescaler. 3686400/64/256 = 225Hz

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11 | 1 << WGM12); //8-bit fastPWM
	TCCR1B |= (1 << CS10) | (1 << CS11);

	OCR0A = 0xFF;
	OCR1A = 0;
	OCR1B = 0;
}

void init_timer(){
	TCCR2B = (1 << CS22) | (1 << CS21); //256 prescaled
	TIMSK2 = (1 << TOIE2);
	TCNT2 = TIMER_TUNE;
}

void init_adc(){
	DDRC |= (1 << PC1); //set voltage divider switch to output

	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE); // /128 + int
	ADMUX = (1 << REFS0); //AVCC as reference
}

void reset_wdt(){
	wdt_reset();
	MCUSR = 0;
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
	wdt_disable();
}

void pwr_measure_on(){
	PORTC &= ~(1 << PC5);
}

void pwr_measure_off(){
	PORTC |= (1 << PC5);
}

void pwr_measure_init(){
	DDRC |= (1 << PC5);
	pwr_measure_on();
}


void init(){
	reset_wdt();
	init_LED();
	uart0_init(UART_BAUD_SELECT(38400UL, F_CPU));
	init_timer();
	init_adc();
	pwr_measure_init();

	OUT_OFF;

	// (PORTD &= ~(1<<4)); //turn off key for BT module
	// wdt_disable();
	// wdt_enable(WDTO_500MS);

	// WDTCSR = (1<<WDIE) | (1<<WDP0) | (1<<WDP1);	//will apply changes in 0.1 second with WDT
	

	// EIMSK = 0x01; //Enable INT0

	// EIMSK = 0x03; //Enable both INT0 and INT1
	// EICRA = 0x08; //INT0 - low level (to be detected in deep sleep), INT1 - on Falling edge (will work when on)
	// EICRA = 0x00; //INT0 & INT1 - low level
	sei();
}
