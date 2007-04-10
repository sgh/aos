
APP = testapp

SOURCES = testgpio.c handlers.c init_cpu.c startarm.s

CFLAGS = -mcpu=arm7tdmi -O0 -ffreestanding -gdwarf-2 -Wall -Wextra -Werror-implicit-function-declaration  -mthumb-interwork
#-mthumb 
#-ffunction-sections -fdata-sections

ASFLAGS = -mcpu=arm7tdmi -mthumb-interwork

LDFLAGS = -nostartfiles  -nodefaultlibs  -nostdlib -mthumb-interwork
#--gc-sections
	
#LINKERSCRIPT = linkerscript.ld
LINKERSCRIPT = lpc2364_rom.ld

LIBS = -laos -lm -lgcc -lnosys -lc

LIBDIRS = -L .

##
# Tools
##
ARM_PREFIX=arm-elf-
GCC = $(ARM_PREFIX)gcc
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
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))


all: ${OBJS}
	$(MAKE) -C src
	$(GCC)  $(CFLAGS) ${LDFLAGS} -Wl,-Map=$(APP).map  -T $(LINKERSCRIPT) $(INCLUDE) -o $(APP).elf ${LIBDIRS} ${OBJS} ${LIBS} 
	$(OBJCOPY) -O ihex $(APP).elf $(APP).hex
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) -t $(OBJS)
	$(SIZE) $(APP).elf

%.o: %.c
	$(GCC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(AS) ${ASFLAGS} $(INCLUDE) -o $@ $<

clean:
	$(MAKE) -C src clean
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f ../$(APP).a

-include kernel/*.d
