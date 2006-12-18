APP = dm

DRIVERS = inclitiometer/sca61t

DRIVER_CLASSES =
DRIVER_BUSSES =

INCLUDE_DIRS = ../include
INCLUDE = $(addprefix -I,$(INCLUDE_DIRS))

SOURCES :=

include $(addsuffix /Makefile,$(DRIVERS))
include $(addsuffix /Makefile,$(DRIVER_CLASSES))
include $(addsuffix /Makefile,$(DRIVER_BUSSES))

SOURCES += main.c ../kernel/list.c driver_core.c driver_class.c

OBJS := $(patsubst %.c,%.o,$(SOURCES))
OBJS := $(patsubst %.s,%.o,$(OBJS))
DEPS := $(patsubst %.o,%.d,$(OBJS))



all: application listmodules

listmodules:
	@echo "Compiled the following : "
	@echo -n "Classes ( "
	@echo $(DRIVER_CLASSES) ")"
	@echo -n "Busses ( "
	@echo $(DRIVER_BUSSES) ")"
#gcc -ggdb -I ../include -I./ -o dm $(SOURCES) -Wall

%.o: %.c
	$(CC) -MD $(CFLAGS) -c $(INCLUDE) -o $@ $<

application: $(OBJS)
	$(CC) $(CFLAGS) -o $(APP) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(APP).elf $(APP).map $(APP).hex $(APP).bin

deps: $(SOURCES)
#$(GCC) -M -E $(SOURCES) $(INCLUDE) > /dev/null

-include kernel/*.d

