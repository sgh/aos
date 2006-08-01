
DRIVERS =

DRIVER_CLASSES =
DRIVER_BUSSES =

SOURCES = main.c ../kernel/list.c spi.c driver_core.c driver_class.c \
					inclitiometer/sca61t/sca61t.c \
					inclitiometer/inclitiometer.c 

include $(addsuffix /depend.mk,$(DRIVERS))

all:
	@echo -n "Classes : "
	@echo $(DRIVER_CLASSES)
	@echo -n "Busses  : "
	@echo $(DRIVER_BUSSES)
	@echo -n "Sources : "
	@echo $(SOURCES)
	gcc -ggdb -I ../include -I./ -o dm $(SOURCES) -Wall
