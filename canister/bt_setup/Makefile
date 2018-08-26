
DEVICE     = atmega168p
CLOCK      = 3686400L
PROGRAMMER = -c usbasp
OBJECTS    = main.o uart.o
BAUD	   = 3
PORT       = /dev/tty.wchusbserial1420

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE) -B $(BAUD) -v


COMPILE = avr-gcc -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -DUART_RX0_BUFFER_SIZE=16 -DUART_TX0_BUFFER_SIZE=128

all: build
	# $(AVRDUDE) -U flash:w:main.hex:i

build:	main.hex main.eep

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse: all
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
