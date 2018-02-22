
DEVICE     = atmega168p
CLOCK      = 3686400L
PROGRAMMER = -c usbasp
OBJECTS    = globals.o main.o uart.o uart_proto.o init.o ds18b20.o adc.o
BAUD	   = 1
PORT       = /dev/tty.wchusbserial1420

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE) -B $(BAUD) -v


COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -DUART_RX0_BUFFER_SIZE=16 -DUART_TX0_BUFFER_SIZE=128

all:	main.hex main.eep

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

bootflash:	all
	#first call is just to cause reset. hack, but working :)
	avrdude -c avr109 -p $(DEVICE) -P $(PORT) -b 38400 -q -n -u -s || true
	avrdude -c avr109 -p $(DEVICE) -P $(PORT) -b 38400 -v -U flash:w:main.hex:i


eeprom:	all
	$(AVRDUDE) -U eeprom:w:main.eep

fuse: all
	#default:
	# $(AVRDUDE) -U lfuse:w:0x62:m -U hfuse:w:0xdf:m
	# $(AVRDUDE) -U efuse:w:0x01:m

	#BOD OFF, WDTOFF 8Mhz
	# $(AVRDUDE) -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0x01:m

	#BOD OFF, WDTON 8Mhz
	# $(AVRDUDE) -U lfuse:w:0xe2:m -U hfuse:w:0xcf:m -U efuse:w:0xf9:m

	#BOD OFF, Crystal, Bootloader On
	$(AVRDUDE) -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xf8:m

	
install: flash fuse

load: all
	bootloadHID main.hex

clean:
	rm -f main.hex main.elf main.eep $(OBJECTS)

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size --format=avr --mcu=$(DEVICE) main.elf

main.eep: main.elf
	rm -f main.eep
	avr-objcopy -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.c
