MCU_UP=t816
MCU=atmega164p
F_CPU=3333333UL
CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
SIZE=avr-size
UP=avrdude

TARGET=main
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU}

PORT=/dev/ttyUSB0
PROGRAMMER-ID=jtag2updi
JTAG-FOLDER=jtag2updi/

all:
	${UP} -C avr/avrdude.conf -p ${MCU_UP} -c ${PROGRAMMER-ID} -P ${PORT} -U flash:w:Debug/kinavr.hex:i

check:
	${CC} ${CFLAGS} -o ${TARGET}.o ${TARGET}.c
	${CC} -o ${TARGET}.elf ${TARGET}.o
	${OBJCOPY} ${HEADER_FLAGS} -j .text -j .data -O ihex ${TARGET}.o ${TARGET}.hex
	${SIZE} -C --mcu=${MCU} ${TARGET}.elf
	rm -f *.o *.elf *.hex

fuse:
	${UP} -C avr/avrdude.conf -p ${MCU_UP} -c ${PROGRAMMER-ID} -P ${PORT} -U fuse2:w:0x01:m
