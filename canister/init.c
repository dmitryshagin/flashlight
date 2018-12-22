#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdio.h>
#include "init.h"
#include "uart.h"

ISR(TIMER2_OVF_vect){ // ~100Hz
	TCNT2 = TIMER_TUNE;
	global_counter++;
}

ISR(INT0_vect){
	cli();

	EIFR = 0b00000011;//lets clear interrupt flags
	
	_delay_ms(1);
	if(PIND & (1<<PIND2)){ //will ignore very short pulses
		sei();
		return;
	}
	EIMSK &= ~(1<<0);

	if(is_on){
		should_on = 0;
		should_off = 1;
	}else{
		should_off = 0;
		should_on = 1;
	}
	sei();
}

ISR(INT1_vect){ //external power is on!
	cli();
	_delay_ms(200);
	if(PIND & (1<<PIND3)){ //will ignore very short pulses
		sei();
		return;
	}
	EIFR = 0b00000011;//lets clear interrupt flags
	EIMSK &= ~(1<<1); //turn off int1. will reenable before sleep
	should_on = 1;
	should_off = 0;
	_delay_ms(2);
	sei();
}


void process_temperature(){
	if(last_processed_counter == global_counter){
		return;
	}
	if(global_counter % 100 == 0){
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temperature );
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );
	}
}


void set_LED(uint8_t R, uint8_t G, uint8_t B){
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
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADIE); // /128 + int
	ADMUX = 0; //AREF reference
}

void reset_wdt(){
	wdt_reset();
	MCUSR &= ~(1<<WDRF);
	cli();
    WDTCSR |= _BV(WDCE) | _BV(WDE);
    WDTCSR = 0;
    sei();
	wdt_disable();
}


void interrupts_init(){
	PORTD |= (1 << PD2) | (1 << PD3); //internal pullups;
	EIMSK = (1<<0); //turn on only INT0. will turn INT1 only before sleep;
	EICRA = 0x00; //INT0 & INT1 - low level
}


void process_leakage(){
	is_leaking = (PINC & (1<<PC3)) ? 0 : 1;
}

void turn_on(){
	cli();
	PRR &= ~(1 << PRUSART0); //reenable uart
	PRR &= ~(1 << PRADC); //reenable adc
	init_timer();
	init_adc();
	uart0_init(UART_BAUD_SELECT(9600UL, F_CPU));
	ds18b20wsp( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, -50, 80, DS18B20_RES12 );
	PORTB |= (1 << PB3); // Pullup for overcurent input pin
	LDO_ON;
	_delay_ms(50); // timeout to settle LDO
	OUT_ON;
	MEASURE_ON;
	DDRD |= (1 << PD5); // BT reset pin -> output
	BT_RESET_HIGH;
	i2c_init();
	is_on = 1;
	wdt_enable(WDTO_4S);
	sei();
}

void turn_off(){
	cli();
	UCSR0B = 0;	//deinit UART
	is_on = 0;
	global_counter = 0;
	adc_ready = 0;
	ADCSRA = 0;
	PRR |= (1 << PRUSART0) | (1 << PRADC); //disable uart & adc
	PORTB &= ~(1 << PB0);//deinit DS18B20
	DDRB &= ~(1 << PB0);//deinit DS18B20
	DDRD &= ~(1 << PD5); // BT reset pin -> input - deiniting
	BT_RESET_LOW;
	set_LED(0,0,0);
	PORTB &= ~(1 << PB3); // Remove pullup for overcurent input pin to reduce power consumption
	OUT_OFF;
	LDO_OFF;
	MEASURE_OFF;
	EIMSK |= (1<<0);
	EIMSK |= (1<<1);
	WDTCSR |= _BV(WDIE);
	sei();
	eeprom_write_word((uint8_t*)0, fram_position);
	reset_wdt();
	sleep_mode();
}



void init(){
	reset_wdt();
	wdt_enable(WDTO_4S);

	cli();

	init_LED();
	interrupts_init();

	DDRB |= (1 << PB4); // Current Measure ouput pin -> output
	DDRB |= (1 << PB5); // Main Output pin -> output
	DDRD |= (1 << PD7); // LDO control output pin -> output
	
	sei();
}
