#include "uart_proto.h"
#include "init.h"
#include "uart.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define MAX_COMMAND_LENGTH UART_RX0_BUFFER_SIZE


uint8_t buffer_pos=0;
static char txBuffer[UART_TX0_BUFFER_SIZE];
static char rxBuffer[MAX_COMMAND_LENGTH];

static void reply_P(const char *addr){
    char c;
    while ((c = pgm_read_byte(addr++)))
    uart0_putc(c);
}

static uint8_t reply_version(){
    reply_P(PSTR("<HW:5.6, SW:0.1.8\n\r"));
    return 0;
}

static uint8_t reply_data(){
    uint32_t data;
    uint16_t i;
    for(i = 0; i < 512; i++){
        wdt_reset();
        data = fram_read((fram_position + i) % 512);
        sprintf(txBuffer, "<D%03d%08lX\n\r", i, data);
        uart0_puts(txBuffer);
        _delay_ms(5);
    }
    return 0;
}

static uint8_t reply_with_status(){
    sprintf(txBuffer, "< %04d, %04d, %04d, %04d, %04d, %04d, %01d\n\r", adc_values[0], adc_values[1], adc_values[2], adc_values[3], adc_values[4], temperature, is_leaking);
    uart0_puts(txBuffer);
    return 0;
}

static uint8_t process_command(uint16_t buffer_pos){
  if(buffer_pos >= MAX_COMMAND_LENGTH||rxBuffer[0]!='>'){
    return 0;
  }
  char * pch = strtok (rxBuffer," ");
  if(pch != NULL){
    if(buffer_pos<2){
        return 0;
    }
    if((strcmp(pch,">?"))==0){ //Get current status
    	return reply_with_status();
    }else if((strcmp(pch,">V"))==0){ //Get current version
        return reply_version();
    }else if((strcmp(pch,">D"))==0){ //Get data logs
        return reply_data();
    }
  }
  return 0;
}

void process_uart(void){
	int c;
	uint8_t i;
	c = uart0_getc();
    if(global_counter - last_data_at > 5){
        buffer_pos = 0;
    }//50 ms timeout btween symbols should be more than enough
    if ( !(c & UART_NO_DATA) )
    {
        if ( c & UART_FRAME_ERROR ){uart0_puts("<!E1 Frame");}
        if ( c & UART_OVERRUN_ERROR ){uart0_puts("<!E2 Overrun");}
        if ( c & UART_BUFFER_OVERFLOW ){uart0_puts("<!E3 Overflow");}
        if(buffer_pos <= 1 && (uint8_t)c == 'S'){ //reset on AVRBOOT commnd to enter bootloader
            reply_P(PSTR("<REBOOT\n\r"));
            wanna_reboot = 1;
            set_LED(0,0,0xFF);
            reset_wdt();
            wdt_enable(WDTO_250MS); // Enable Watchdog Timer to give reset
        }
        if ((uint8_t)c == '>'){
        	for(buffer_pos=MAX_COMMAND_LENGTH-1;buffer_pos>0;buffer_pos--){rxBuffer[buffer_pos]=0;}
        	rxBuffer[buffer_pos++]=c;
        }else if((c=='\0'||c=='\r')){
            // set_LED(0xFF,0,0);
        	process_command(buffer_pos);
	        for(i=0;i<MAX_COMMAND_LENGTH;i++){rxBuffer[i]=0;}
        }else{
        	rxBuffer[buffer_pos++]=c;
        }
        if(buffer_pos==MAX_COMMAND_LENGTH){ //reset buffer on erraty command
        	for(buffer_pos=MAX_COMMAND_LENGTH-1;buffer_pos>0;buffer_pos--){rxBuffer[buffer_pos]=0;}
        }
        last_data_at = global_counter;
    }
}
