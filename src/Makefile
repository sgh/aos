.SILENT: deps

APP = armOS


KERNEL_SOURCES = main.c startup.s context.s syscalls.c list.c timer_interrupt.c semaphore.c interrupt.c uart.c mutex.c spinlock.c atomic.s kernel.c

DRIVER_SOURCES =  driver_core.c driver_class.c serio/serio.c platform/platform.c

MM_SOURCES = mm.c fragment.c 

SOURCES = $(addprefix kernel/,$(KERNEL_SOURCES))  $(addprefix memory/,$(MM_SOURCES))  $(addprefix driver-model/,$(DRIVER_SOURCES))

CFLAGS = -mcpu=arm7tdmi -mapcs -O0 -nostartfiles -nostdlib -ffreestanding -gdwarf-2 -Wall -Werror-implicit-function-declaration
#-ffunction-sections -fdata-sections

LDFLAGS = 
#--gc-sections
	
LINKERSCRIPT = kernel/linkerscript.ld

LIBS = -lc -lm -lgcc -lnosys

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
INCLUDE_DIRS = include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))


all: deps application
	$(GCC) $(CFLAGS) -Wl,-Map=$(APP).map  -T $(LINKERSCRIPT) -o $(APP).elf $(OBJS) $(LIBS)
	$(OBJCOPY) -O ihex $(APP).elf $(APP).hex
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) -t $(OBJS)
	$(SIZE) $(APP).elf

%.o: %.c
	$(GCC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(AS) -MD -c $(INCLUDE) -o $@ $<

application: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(APP).elf $(APP).map $(APP).hex $(APP).bin

deps: $(SOURCES)
	#$(GCC) -M -E $(SOURCES) $(INCLUDE) > /dev/null

-include kernel/*.d
