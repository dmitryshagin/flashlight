#include "uart_proto.h"
#include "init.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define MAX_COMMAND_LENGTH UART_RX0_BUFFER_SIZE


uint8_t buffer_pos=0;


static char rxBuffer[MAX_COMMAND_LENGTH];

static void reply_P(const char *addr){
    char c;
    while ((c = pgm_read_byte(addr++)))
    uart0_putc(c);
}

static void reply_OK(){
    reply_P(PSTR("<OK\n\r"));
}

static uint8_t reply_with_status(){
    char _tmpstr[UART_TX0_BUFFER_SIZE];
    sprintf(_tmpstr, "< %04d, %04d, %04d, %04d\n\r", adc_values[0], adc_values[1], adc_values[2], temp);
    uart0_puts(_tmpstr);
    return 0;
}

static uint8_t temp_to_test(){
    reply_OK();
    return 0;
}

static uint8_t processCommand(uint16_t buffer_pos){
  if(buffer_pos>=MAX_COMMAND_LENGTH||rxBuffer[0]!='>'){
    return 0;
  }
  char * pch = strtok (rxBuffer," ");
  if(pch != NULL){
    if(buffer_pos<2){
        return 0;
    }
    if(rxBuffer[1]=='S'){
        if((strcmp(pch,">SB"))==0){ //Set something useful. Just for test
            return temp_to_test(pch);
        }
    }else if((strcmp(pch,">?"))==0){ //Get current status
    	return reply_with_status();
    }
  }
  return 0;
}

void process_uart(void){
	int c;
	uint8_t i;
	c = uart0_getc();
    if ( !(c & UART_NO_DATA) )
    {
        if ( c & UART_FRAME_ERROR ){uart0_puts("<!E1 Frame");}
        if ( c & UART_OVERRUN_ERROR ){uart0_puts("<!E2 Overrun");}
        if ( c & UART_BUFFER_OVERFLOW ){uart0_puts("<!E3 Overflow");}
        if(buffer_pos <= 1 && (uint8_t)c == 'S'){ //reset on AVRBOOT commnd to enter bootloader
            setLED(0xFF,0,0);
            wdt_enable(WDTO_250MS); // Enable Watchdog Timer to give reset
        }
        if ((uint8_t)c == '>'){
        	for(buffer_pos=MAX_COMMAND_LENGTH-1;buffer_pos>0;buffer_pos--){rxBuffer[buffer_pos]=0;}
        	rxBuffer[buffer_pos++]=c;
        }else if((c=='\0'||c=='\r')){
            setLED(0xFF,0,0);
        	processCommand(buffer_pos);
	        for(i=0;i<MAX_COMMAND_LENGTH;i++){rxBuffer[i]=0;}
                setLED(0,0,0);
        }else{
        	rxBuffer[buffer_pos++]=c;
        }
        if(buffer_pos==MAX_COMMAND_LENGTH){ //reset buffer on erraty command
        	for(buffer_pos=MAX_COMMAND_LENGTH-1;buffer_pos>0;buffer_pos--){rxBuffer[buffer_pos]=0;}
        }
    }
}
