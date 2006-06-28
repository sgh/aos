.SILENT: deps

SOURCES = main.c startup.s context.s syscalls.c queue.c timer_interrupt.c semaphore.c vic.c uart.c mutex.c spinlock.c atomic.s kernel.c

CFLAGS = -mcpu=arm7tdmi -mapcs -O0 -nostartfiles -nostdlib -ffreestanding -gdwarf-2 -Wall 
# -ffunction-sections -fdata-sections

LDFLAGS = 
#--gc-sections
	
LINKERSCRIPT = linkerscript.ld

LIBS = -lc -lm -lgcc

##
# Tools
##
ARM_PREFIX=arm-elf
GCC = $(ARM_PREFIX)-gcc
AS = $(ARM_PREFIX)-as
LD = $(ARM_PREFIX)-ld
SIZE = $(ARM_PREFIX)-size
OBJCOPY = $(ARM_PREFIX)-objcopy

##
# Includes
##
INCLUDE_DIRS = ../include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))


all: deps  mmsOS
	$(GCC) $(CFLAGS) -Wl,-Map=mmsOS.map  -T $(LINKERSCRIPT) -o mmsOS.elf $(OBJS) $(LIBS)
	$(OBJCOPY) -O ihex mmsOS.elf mmsOS.hex
	$(OBJCOPY) -O binary mmsOS.elf mmsOS.bin
	cp mmsOS.elf kernel.elf

flash: all
	lpcflash.sh mmsOS.hex

%.o: %.c
	$(GCC) $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(GCC) $(CFLAGS) -c $(INCLUDE) -o $@ $<

mmsOS: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f *.elf

deps: $(SOURCES)
	$(GCC) -MD -E $(SOURCES) $(INCLUDE) > /dev/null

-include *.d
