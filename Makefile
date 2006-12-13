
APP = testapp

SOURCES = testapp.c startup.s

CFLAGS = -mcpu=arm7tdmi -mapcs -O0 -ffreestanding -gdwarf-2 -Wall -Wextra -Werror-implicit-function-declaration  -ffast-math -fno-math-errno 
#-ffunction-sections -fdata-sections

LDFLAGS = -nostartfiles  -nodefaultlibs  -nostdlib --gc-sections
	
LINKERSCRIPT = linkerscript.ld

LIBS =  -laos -lm -lgcc -lnosys -lc

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
INCLUDE_DIRS = src/include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))


all: application ${OBJS}
	$(GCC) ${LDFLAGS} -Wl,-Map=$(APP).map  -T $(LINKERSCRIPT) $(INCLUDE) -o $(APP).elf ${LIBDIRS} ${OBJS} ${LIBS} 
	$(OBJCOPY) -O ihex $(APP).elf $(APP).hex
	$(OBJCOPY) -O binary $(APP).elf $(APP).bin
	$(SIZE) -t $(OBJS)
	$(SIZE) $(APP).elf

%.o: %.c
	$(GCC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

%.o: %.s
	$(AS) $(INCLUDE) -o $@ $<

application: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f ../$(APP).a

-include kernel/*.d
