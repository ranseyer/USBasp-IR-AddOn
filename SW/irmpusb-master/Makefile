# Name: Makefile
# Author: Christian Starkjohann
# Tabsize: 4
# Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GPLv2.
#
# Makefile taken from easylogger

DEVICE=atmega8
F_CPU=12000000

#AVRDUDE = avrdude -c stk500v2 -p $(DEVICE) -P /dev/ttyUSB1
AVRDUDE = avrdude -c usbasp -p $(DEVICE) -P /dev/ttyUSB0

# Choose your favorite programmer and interface.

COMPILE = avr-gcc -Wall -Os -Iusbdrv -Iirmp -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DDEBUG_LEVEL=0

# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

#OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o irmp/irmp.c libs-device/osccal.o main.o
#OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o irmp/irmp.c main.o
OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o irmp/irmp.c main.o

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
ifeq ($(DEVICE),atmega8)
	$(AVRDUDE) -U hfuse:w:0xc6:m -U lfuse:w:0x9f:m		
else
	# Safety mode, dont kill your AVR :)
	# FUSE NOT SET, please check for your device!!
endif

readcal:
	$(AVRDUDE) -U calibration:r:/dev/stdout:i | head -1


clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o usbdrv/*.o main.s usbdrv/oddebug.s usbdrv/usbdrv.s libs-device/osccal.o

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	#./checksize main.bin 8192 512
	avr-size main.bin
# do the checksize script as our last action to allow successful compilation
# on Windows with WinAVR where the Unix commands will fail.

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
