###############################################################################
# Makefile for the project DualChannelCounter1PWM
###############################################################################

## General Flags
PROJECT = DualChannelCounter1PWM
MCU = atmega32
TARGET = DualChannelCounter1PWM.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DBAUD=57600   -DSIMPLEPROTOCOL_DEVICEADDRESS=0x01                                                         -DF_CPU=16000000UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -lm -Wl,-Map=DualChannelCounter1PWM.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = usart.o packets.o queue.o crc16.o pwm.o time.o commands.o dualchannelcounter1pwm.o motors.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) DualChannelCounter1PWM.hex DualChannelCounter1PWM.eep DualChannelCounter1PWM.lss size

## Compile
usart.o: ../../lib/src/usart.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

packets.o: ../../lib/src/packets.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

queue.o: ../../lib/src/queue.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

crc16.o: ../../lib/src/crc16.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwm.o: ../../lib/src/pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

time.o: ../../lib/src/time.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

commands.o: ../src/commands.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

dualchannelcounter1pwm.o: ../src/dualchannelcounter1pwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

motors.o: ../src/motors.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) DualChannelCounter1PWM.elf dep/* DualChannelCounter1PWM.hex DualChannelCounter1PWM.eep DualChannelCounter1PWM.lss DualChannelCounter1PWM.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

