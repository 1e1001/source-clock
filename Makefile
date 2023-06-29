TARGETS = target/libi2c/twi/twi_master.o target/libuart/uart.o target/main.o

CFLAGS = -Wall -Wextra -std=c99 -pedantic -Wno-array-bounds
CFLAGS += -DF_CPU=16000000UL -DBAUDRATE=9600

screen: program
	screen ${DEV} 9600

program: build
	avrdude -p m328p -P ${DEV} -c arduino -U flash:w:target/flash.hex

target/%.o: %.c
	avr-gcc -Os ${CFLAGS} -mmcu=atmega328p -c $< -o $@

target/flash.hex: ${TARGETS}
	avr-gcc ${CFLAGS} -mmcu=atmega328p -o target/flash.elf ${TARGETS}
	avr-objcopy -O ihex target/flash.elf $@

build: target/flash.hex

clean:
	rm -rfv target
	for dir in ${TARGETS}; do mkdir -vp $$(dirname $$dir); done
