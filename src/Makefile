
APP = libaos

MARCH = arm-lpc2xxx

KERNEL_SOURCES = syscalls.c list.c timer_interrupt.c semaphore.c mutex.c spinlock.c kernel.c task.c uart.c

DRIVER_SOURCES =  driver_core.c driver_class.c platform/platform.c

ARCH_SOURCES = irq.c context.s atomic.s

MM_SOURCES = mm.c fragment.c 

SOURCES = $(addprefix kernel/,$(KERNEL_SOURCES))  $(addprefix memory/,$(MM_SOURCES))  $(addprefix drivers/,$(DRIVER_SOURCES)) $(addprefix arch/$(MARCH)/,$(ARCH_SOURCES))

CFLAGS = -mcpu=arm7tdmi -mapcs -Os -nostartfiles -nostdlib -ffreestanding -gdwarf-2 -Wall -Wextra -Werror-implicit-function-declaration -mthumb-interwork -mthumb
#-ffunction-sections -fdata-sections

LDFLAGS = -mthumb-interwork

ASFLAGS = -mthumb-interwork
#--gc-sections
	
LINKERSCRIPT = kernel/linkerscript.ld

#LIBS = -lc -lm -lgcc -lnosys 
LIBS = 

LIBDIRS = -L .

##
# Tools
##
ARM_PREFIX=arm-elf-
GCC = $(ARM_PREFIX)gcc
AS = $(ARM_PREFIX)as
AR = $(ARM_PREFIX)ar
LD = $(ARM_PREFIX)ld
SIZE = $(ARM_PREFIX)size
OBJCOPY = $(ARM_PREFIX)objcopy

##
# Includes
##
INCLUDE_DIRS = include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))


all: application
	$(AR) rcs ../$(APP).a $(OBJS)
	#${AR}  t lib${APP}.a 
	#$(GCC) $(CFLAGS) -Wl,-Map=$(APP).map  -T $(LINKERSCRIPT) $(INCLUDE) -o $(APP).elf kernel/main.c ${LIBDIRS} ${LIBS}
	#$(OBJCOPY) -O ihex $(APP).elf $(APP).hex
	#$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) -t $(OBJS)
	#$(SIZE) $(APP).elf

%.o: %.c
	$(GCC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) $(INCLUDE) -o $@ $<

application: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f ../$(APP).a

-include kernel/*.d
