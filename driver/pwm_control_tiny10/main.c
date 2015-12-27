#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h> 

#define LED_LOW_VOLTAGE 0xFF>>2
#define LED_50_PERCENT  0xFF>>1
#define LED_100_PERCENT 0xFF
#define ADC_LIMIT 0xFF>>1

volatile uint8_t is_sleeping;
volatile uint8_t current_mode;
volatile uint8_t to_apply;

// PB0 - ADC in; PB1 - PWM out; PB2 - INTERRUPUT IN

ISR(INT0_vect){
	cli();
	ADCSRA = 0; //АЦП вырубим
	EIMSK = 0; //вырубим INT0 (дабы избежать дребезга)
	if (PINB & (1<<PB2)){ //высокий уровень PB2 - значит у нас пропало питание
		EICRA = 0; //перенастроим прерывание по INT0 на низкий уровень для определения наличия питания
		is_sleeping = 1; //флажок, по которому главный цикл погонит контроллер в спячку
		current_mode = 1-current_mode; //после просыпания нужен будет другой режим, выберем его сразу
	}else{ //низкий уровень, значит питание вернулось
		EICRA = (1<<ISC00)|(1<<ISC01);//переключим прерывание на высокий уровень		
		is_sleeping = 0; //поставим флажок, что пора просыпаться
	}
	WDTCSR = (1<<WDIE) | (1<<WDP0) | (1<<WDP1); //поставим WDT на 0.1с, чтобы подождать стабилизации питания 
	
}

ISR(WDT_vect){
	cli(); // прошло время антидребезга - врубим прерывания обратно, применим изменения
	if(!is_sleeping){
		to_apply = 1;//надо применить новый режим
	}	
	WDTCSR = 0; //вырубим WDT, он больше пока не нужен
	EIMSK = (1<<INT0);//так как WDT только на антидребезг работает, то выключим его и включим обратно INT0
}

ISR(ADC_vect){
	ADCSRA |= 1<<ADSC;		// Start Conversion
	if(ADCL<ADC_LIMIT){
		OCR0B = LED_LOW_VOLTAGE;
	}
}


int main(void){

	DDRB  = (0<<DDB0) | (1<<DDB1) | (0<<DDB2) | (0<<DDB3); //первая нога - выход? остальные - вход

	EICRA = (1<<ISC00)|(1<<ISC01);//разрешаем int0 по смене сигнала (для определения выключения питания)
	EIMSK = (1<<INT0);

	TCCR0A = (1 << COM0B1) | (1 << WGM00);  // phase correct PWM mode
    TCCR0B = (1 << CS01);   // clock source = CLK/8, start PWM
    OCR0B = LED_100_PERCENT;

    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADIE);             // enable ADC on 1mhz/16 with interrupt
    ADCSRA |= (1<<ADSC); //start conversion

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	while(1){
		sei();
		if(is_sleeping){
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