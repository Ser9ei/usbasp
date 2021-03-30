#
#   Makefile for usbasp
#   20061119   Thomas Fischl        original
#   20061120   Hanns-Konrad Unger   help: and TARGET=atmega48 added
#   20201124   Ser9ei v-usb updated, usbisp support
#   20201125   Ser9ei merge changes from https://github.com/nofeletru/UsbAsp-flash by Tify (25-flash, 24-eeprom, 93-eeprom support)
#   20201126   Ser9ei merge changes from TTAV134 NeoProgrammer project in part of flash&eerprom. This version is for testing purposes only usbisp programmer with NeoProgrammer
#   20210330   Ser9ei Build error under the Windows OS was fixed in the Makefile, 
#                     Optimization parameter was changed in the Makefile, 
#                     Added auto-speed (downshift speed) support feature in case error with connection to the target 
				

# TARGET=atmega8    HFUSE=0xc9  LFUSE=0xef
# TARGET=atmega48   HFUSE=0xdd  LFUSE=0xff
# TARGET=atmega88   HFUSE=0xdd  LFUSE=0xff
TARGET=atmega8

ifeq ($(TARGET), atmega8) 
	HFUSE=0xc9
	LFUSE=0xef
else ifeq ($(TARGET), atmega48) 
	HFUSE=0xdd
	LFUSE=0xff
else ifeq ($(TARGET), atmega88)
	HFUSE=0xdd
	LFUSE=0xff
endif


#
#usbasp
#usb D+ -> 32 pd2(int0)
#usb D- -> 12 pb0(icp1)
#
#usbisp
#usb D+ -> 32 pd2(int0)
#usb D- -> 1  pd3(int1)
#


# ISP=bsd      PORT=/dev/parport0
# ISP=ponyser  PORT=/dev/ttyS1
# ISP=stk500   PORT=/dev/ttyS1
# ISP=usbasp   PORT=/dev/usb/ttyUSB0
# ISP=stk500v2 PORT=/dev/ttyUSB0
ISP=usbasp
PORT=usb
FLASHSPEED=1.0
# FLASHSPEED=0.5 1.5Mhz
# FLASHSPEED=1.0 750Khz
# FLASHSPEED=2.0 375Khz
# FLASHSPEED=4.0 187.5Khz
# FLASHSPEED=8.0 93.75Khz

out_name=usbasp
out_name:=$(out_name)_$(TARGET)
ifeq ($(OS),Windows_NT)
	out_name:=$(out_name)_$(shell date.exe +%Y-%m-%d)
else
	out_name:=$(out_name)_$(shell date +%Y-%m-%d)
endif
out_name:=$(out_name).hex

all: clean main.hex
	rm -f $(out_name)
	cp main.hex $(out_name) 
	@avr-size -C --mcu=$(TARGET) main.bin

program: fuses flash 

help:
	@echo "Usage: make                same as make help"
	@echo "       make help           same as make"
	@echo "       make all            clean and build firmware"
	@echo "       make program        program fuses and flash"
	@echo "       make clean          remove redundant data"
	@echo "       make main.hex       create main.hex"
	@echo "       make disasm         disasm main"
	@echo "       make flash          upload main.hex into flash"
	@echo "       make fuses          program fuses"
	@echo "       make avrdude        test avrdude"
	@echo "Current values:"
	@echo "       TARGET=${TARGET}"
	@echo "       LFUSE=${LFUSE}"
	@echo "       HFUSE=${HFUSE}"
	@echo "       CLOCK=12000000"
	@echo "       ISP=${ISP}"
	@echo "       PORT=${PORT}"
	@echo "       FLASHSPEED=${FLASHSPEED}"

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=$(TARGET) # -DDEBUG_LEVEL=2 

OBJECTS = usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o isp.o clock.o tpi.o i2c.o microwire.o main.o

.c.o:
	$(COMPILE) -c $< -o $@
#-Wa,-ahlms=$<.lst

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.bin *.o main.s usbdrv/*.o

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS) -Wl,-Map,main.map

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c

flash:
	avrdude -c ${ISP} -B ${FLASHSPEED} -e -p ${TARGET} -P ${PORT} -U flash:w:main.hex

fuses:
	avrdude -c ${ISP} -p ${TARGET} -P ${PORT} -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

avrdude:
	avrdude -c ${ISP} -p ${TARGET} -P ${PORT} -v

# Fuse atmega8 high byte HFUSE:
# 0xc9 = 1 1 0 0   1 0 0 1 <-- BOOTRST (boot reset vector at 0x0000)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   + --------- EESAVE (don't preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (full output swing)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ WDTON (WDT not always on)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse atmega8 low byte LFUSE:
# 0x9f = 1 0 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ BODEN (BrownOut Detector enabled)
#        +-------------------- BODLEVEL (2.7V)
#
# Fuse atmega48 high byte hfuse:
# 0xdf = 1 1 0 1   1 1 1 1     factory setting
#        ^ ^ ^ ^   ^ \-+-/
#        | | | |   |   +------ BODLEVEL (Brown out disabled)
#        | | | |   + --------- EESAVE (don't preserve EEPROM over chip erase)
#        | | | +-------------- WDTON (WDT not always on)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ DWEN (debug wire is disabled)
#        +-------------------- RSTDISBL (reset pin is enabled)
# 0xdd = ext.reset, no DW, SPI, no watchdog, no save eeprom, BOD 2.7V
# Fuse atmega48 low byte lfuse:
# 0x62 = 0 1 1 0   0 0 1 0     factory setting
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (internal 8Mhz Oszillator)
#        | |  +--------------- SUT 1..0 (start-up time)
#        | +------------------ CKOUT (no clock output)
#        +-------------------- CKDIV8 (divide clock by 8)
# 0xdc = divide/1,no clock output,fast raising power,low Pw Oszil. 3..8 Mhz
# 0xe0 = divide/1,no clock output,fast raising power,external Oszil.
# 0xff = divide/1,no clock output,slow raising power,low Pw Oszil 8..  Mhz


SERIAL = `echo /dev/tty.USA19QI*`
UISP = uisp -dprog=$S -dserial=$(SERIAL) -dpart=auto
# The two lines above are for "uisp" and the AVR910 serial programmer connected
# to a Keyspan USB to serial converter to a Mac running Mac OS X.
# Choose your favorite programmer and interface.

uisp:	all
	$(UISP) --erase
	$(UISP) --upload --verify if=main.hex

