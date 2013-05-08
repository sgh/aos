.PHONY: all symlink_setup clean

APP = ../libaos.a

KERNEL_SOURCES = input.c input_events.c aosfifo.c

ifeq ($(ARCH),arm-lpc2xxx)
	ARCH_SOURCES = interrupt.c context.c clock.c core.s
	KERNEL_SOURCES += assert.c syscalls.c list.c semaphore.c mutex.c task.c kernel.c sched.c irq.c timer.c
	MM_SOURCES = mm.c fragment.c newdelete.c
	ASMLIST = -Wa,-alhmsd
	FLAGS = -mcpu=arm7tdmi -O2 -nostartfiles -nostdlib -gdwarf-2 -Wall -Wextra -mthumb-interwork $(ASMLIST)=$@.lst
	CFLAGS = $(FLAGS) -Werror-implicit-function-declaration -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes
	CPPFLAGS = $(FLAGS) 
	THUMB = -mthumb
	LDFLAGS = -mthumb-interwork
	ASFLAGS = -mthumb-interwork -gdwarf2
endif

ifeq ($(ARCH),linux)
	ARCH_SOURCES = mutex.c kernel.c
	CFLAGS = -m32
	CPPFLAGS = -m32
endif

ifeq ($(ARCH),)
	ARCH_ERROR = $(error error Please set ARCH to (arm-lpc2xxx|linux))
endif

CC      = $(TOOLCHAIN_PREFIX)gcc
AS      = $(TOOLCHAIN_PREFIX)as
AR      = $(TOOLCHAIN_PREFIX)ar
LD      = $(TOOLCHAIN_PREFIX)ld
SIZE    = $(TOOLCHAIN_PREFIX)size
OBJCOPY = $(ARM_PREFIX)objcopy

SOURCES = $(addprefix kernel/,$(KERNEL_SOURCES)) $(addprefix memory/,$(MM_SOURCES)) $(addprefix arch/$(ARCH)/,$(ARCH_SOURCES))

##
# Tools
##


##
# Includes
##
INCLUDE_DIRS = include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))


OBJS := $(patsubst %.cpp,%.o,$(SOURCES))
OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))

$(APP): include/arch $(OBJS)
	@echo "Creating $(APP) ..."
	@$(AR) rcs $(APP) $(OBJS)

size:
	@$(SIZE) -t $(OBJS)

symlink_setup: include/arch

include/arch:
	$(ARCH_ERROR)
	ln -sfn ../arch/$(ARCH) include/arch

lint:
	$(MAKE) CC=splint CFLAGS=$(INCLUDE)

%.o: %.cpp
	@echo "Compiling (C++) $< ..."
	@$(CC) -MD $(THUMB) $(CPPFLAGS) -c $(INCLUDE) -o $@ $<

kernel/sched.o:  kernel/sched.c
	@echo "Compiling (ARM) $<  ..."
	@$(CC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

kernel/irq.o:  kernel/irq.c
	@echo "Compiling (ARM) $<  ..."
	@$(CC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.c
	@echo "Compiling (C) $< ..."
	@$(CC) -MD $(THUMB) $(CFLAGS) -c $(INCLUDE) -o $@ $<


%.o: %.s
	@echo "Assembling (ARM) $< ..."
	@$(AS) $(ASFLAGS) $(INCLUDE) -o $@ $<


clean:
	rm -f include/arch $(OBJS) $(DEPS) $(APP)

-include $(DEPS)
