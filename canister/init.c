#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include "init.h"
#include "uart.h"

ISR(TIMER2_OVF_vect){ // ~100Hz
	TCNT2 = TIMER_TUNE;
	global_counter++;
}

ISR(INT0_vect){
	cli();
	EIMSK &= ~(1<<0);
	
	_delay_ms(5);
	if(PIND & (1<<PIND2)){ //will ignore very short pulsed
		return;
	}
	OUT_INVERT;
	_delay_ms(1);
	if( (PINC & (1<<PC2)) ){
		turn_everything_on();
	}
	processLED();
	wdt_enable(WDTO_4S);
	while(!(PIND & (1<<PIND2))){_delay_ms(50);}
	reset_wdt();
	if( (PINC & (1<<PC2)) ){
		turn_everything_on();
	}
	_delay_ms(50); //we dont want to swith too often;
	sei();
}

ISR(INT1_vect){ //external power is on!
	cli();
	EIMSK &= ~(1<<1); //turn off int1. TODO: reenable before sleep!
	OUT_ON;
	_delay_ms(2);
	sei();
}


void processTemp(){
	if(last_processed_counter == global_counter){
		return;
	}
	if(global_counter % 100 == 0){
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );
	}
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
}

void interrupts_init(){
	PORTD |= (1 << PD2) | (1 << PD3); //internal pullups;
	EIMSK = (1<<0); //turn on only INT0. will turn INT1 only before sleep;
	EICRA = 0x00; //INT0 & INT1 - low level
}

void init_leakage(){
	PORTC |= (1 << PC4);
}

void processLeakage(){
	leakage = (PINC & (1<<PC3)) ? 0 : 1;
}

void bt_init(){
	DDRD |=  (1 << PD4);
	DDRD |=  (1 << PD5);
	DDRD |=  (1 << PD7);
	BT_PWR_ON;
	BT_CMD_OFF;
	BT_RESET_OFF;
}

void turn_everything_on(){
	init_timer();
	init_adc();
	WTR_SENS_ON;
	pwr_measure_on();
	bt_init();
}

void turn_everything_off(){
	WTR_SENS_OFF;
	pwr_measure_off();
	BT_PWR_OFF;
	setLED(0,0,0);
}

void init(){
	reset_wdt();
	init_LED();
	uart0_init(UART_BAUD_SELECT(38400UL, F_CPU));
	pwr_measure_init();
	ds18b20wsp( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, -50, 80, DS18B20_RES12 );
	init_leakage();
	interrupts_init();

	turn_everything_on();

	OUT_OFF;

	sei();
}
