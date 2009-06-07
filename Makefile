.PHONY:  aos
APP = usbdevice

#SOURCES = testgpio.c handlers.c init_cpu.c startarm.s
#SOURCES = startarm.s testapp.c init_cpu_lpc21xx.c handlers.c
SOURCES = startarm.s usbdevice.cpp init_cpu.c handlers.c 

CFLAGS = -mcpu=arm7tdmi-s -Os -gdwarf-2 -Wall -Wextra  -mthumb-interwork -fno-rtti -fno-exceptions -std=c99
#-mthumb 
#-ffunction-sections -fdata-sections

ASFLAGS = -mcpu=arm7tdmi -mthumb-interwork

#LDFLAGS = -g -nostartfiles  -nodefaultlibs  -nostdlib -mthumb-interwork
LDFLAGS = -g -nostartfiles  -mthumb-interwork
#--gc-sections
	
LINKERSCRIPT = linkerscript.ld
#LINKERSCRIPT = lpc2364_rom.ld

LIBS = -laos -lm -lgcc -lc -lstdc++

LIBDIRS = -L .

##
# Tools
##
ARM_PREFIX=arm-elf-
GCC = $(ARM_PREFIX)gcc
GXX = $(ARM_PREFIX)g++
AS = $(ARM_PREFIX)as
AR = $(ARM_PREFIX)ar
LD = $(ARM_PREFIX)ld
STRIP = $(ARM_PREFIX)strip
SIZE = $(ARM_PREFIX)size
OBJCOPY = $(ARM_PREFIX)objcopy

##
# Includes
##
INCLUDE_DIRS = src/include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.cpp,%.o,$(OBJS))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))



all: ${OBJS}
	$(GXX) $(CFLAGS) ${LDFLAGS} -Wl,-Map=$(APP).map  -T $(LINKERSCRIPT) $(INCLUDE) -o $(APP).elf ${LIBDIRS} ${OBJS} ${LIBS} 
	$(OBJCOPY) -O ihex $(APP).elf $(APP).hex
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) -t $(OBJS)
	$(SIZE) $(APP).elf
#cp ../Mikrofyn/Embedded/arm/xc2-bootcode/cb14_cradle-bootloader.bin . 
#dd if=testapp.bin bs=8k seek=1 skip=1 oflag=append of=cb14_cradle-bootloader.bin
#$(OBJCOPY) -I binary -O ihex cb14_cradle-bootloader.bin cb14_cradle-bootloader.hex
	$(OBJCOPY) -O ihex $(APP).elf  testapp.hex


aos:
	$(MAKE) -C src 


%.o: %.c
	$(GCC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.cpp
	$(GXX) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(AS) ${ASFLAGS} $(INCLUDE) -o $@ $<

clean:
	$(MAKE) -C src clean
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f ../$(APP).a

-include *.d
