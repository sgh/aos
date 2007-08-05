.SILENT: symlink_setup
.PHONY: all symlink_setup clean application

APP = libaos

MARCH = arm-lpc2xxx

KERNEL_SOURCES = syscalls.c list.c semaphore.c mutex.c task.c kernel.c timer_interrupt.c wait.c irq.c timer.c runtime_check.c assert.c

DRIVER_SOURCES =  
#driver_core.c driver_class.c platform/platform.c

ARCH_SOURCES = interrupt.c context.s atomic.s clock.c

MM_SOURCES = mm.c fragment.c 

ARM_SOURCES = $(addprefix kernel/,$(ARM_KERNEL_SOURCES))

SOURCES = $(addprefix kernel/,$(KERNEL_SOURCES)) $(addprefix memory/,$(MM_SOURCES))  $(addprefix drivers/,$(DRIVER_SOURCES)) $(addprefix arch/$(MARCH)/,$(ARCH_SOURCES))

ARM_SOURCES = $(addprefix kernel/,$(ARM_KERNEL_SOURCES))

ASMLIST = -Wa,-alhmsd

CFLAGS = -mcpu=arm7tdmi -O2 -nostartfiles -nostdlib -gdwarf-2 -Wall -Wextra -Werror-implicit-function-declaration -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes -mthumb-interwork 
#-mapcs
#-ffreestanding
#-ffunction-sections -fdata-sections

THUMB =
#-mthumb

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

ARM_OBJS := $(patsubst %.c,%.o,$(ARM_SOURCES))
OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))

all: symlink_setup application
	@echo "Creating $(APP).a ..."
	@$(AR) rcs ../$(APP).a $(OBJS)
	@$(SIZE) -t $(OBJS)

symlink_setup:
	ln -sfn ../arch/$(MARCH) include/arch

lint:
	$(MAKE) GCC=splint CFLAGS=$(INCLUDE)

%.o: %.c
	@echo "Compiling $< ..."
	@$(GCC) -MD $(THUMB) $(CFLAGS) $(ASMLIST)=$@.lst -c $(INCLUDE) -o $@ $<

kernel/timer_interrupt.o:  kernel/timer_interrupt.c
	@echo "Compiling (ARM) $<  ..."
	@$(GCC) -MD $(CFLAGS) $(ASMLIST)=$@.lst -c $(INCLUDE) -o $@ $<

%.o: %.s
	@echo "Assembling (ARM) $< ..."
	@$(AS) $(ASFLAGS) $(INCLUDE) -o $@ $<

application: $(OBJS)

clean:
	rm -f include/arch $(OBJS) $(DEPS) ../$(APP).a

-include $(DEPS)
