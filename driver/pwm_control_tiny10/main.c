#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 

#define LED_LOW_VOLTAGE 0xFF>>2
#define LED_50_PERCENT  0xFF>>1
#define LED_100_PERCENT 0xFF
#define ADC_LIMIT 0xFF>>1

volatile uint8_t need_to_sleep, current_mode, to_apply;

ISR(INT0_vect){
	cli();
	ADCSRA = 0;		//turning off ADC
	EIMSK = 0;		//turning off INT0
	if (PINB & (1<<PB2)){	//high level => we've got power loss! let's go to sleep
		EICRA = 0;			//INT0 will now trigger on low level (only possible option in PWD down)
		need_to_sleep = 1;
		current_mode = 1-current_mode;
	}else{			//low level => we've got power back :) lets' wake up
		EICRA = (1<<ISC00)|(1<<ISC01);	//INT0 will now trigger on rising edge to detect power loss
		need_to_sleep = 0;
	}
	WDTCSR = (1<<WDIE) | (1<<WDP0) | (1<<WDP1);	//will apply changes in 0.1 second with WDT
}

ISR(WDT_vect){
	cli();
	if(!need_to_sleep){
		to_apply = 1;
	}	
	WDTCSR = 0;			//don't need WTD - let's turn on INT0 back on
	EIMSK = (1<<INT0);
}

ISR(ADC_vect){
	ADCSRA |= 1<<ADSC;	//start conversion
	if(ADCL<ADC_LIMIT){
		OCR0B = LED_LOW_VOLTAGE;
	}
}


int main(void){
	// PB0 - ADC in; PB1 - PWM out; PB2 - INTERRUPUT IN, PB3 - unused;
	DDRB  = (0<<DDB0) | (1<<DDB1) | (0<<DDB2) | (0<<DDB3);

	EICRA = (1<<ISC00) | (1<<ISC01);				//INT0 by rising edge to detect power loss
	EIMSK = (1<<INT0);

	TCCR0A = (1 << COM0B1) | (1 << WGM00);  		// PWM mode
	TCCR0B = (1 << CS01);							// clock source = CLK/8, start PWM
	OCR0B = LED_100_PERCENT;

	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADIE);	// enable ADC on 1mhz/16 with interrupt
	ADCSRA |= (1<<ADSC);							//start conversion

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	while(1){
		sei();
		if(need_to_sleep){
			sleep_mode();
		}
		if(to_apply==1){
			if(current_mode == 1){
				OCR0B = LED_50_PERCENT;
			}else{
				OCR0B = LED_100_PERCENT;
			}
			to_apply = 0;
		}
		
	}
}