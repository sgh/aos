
SOURCES = $(wildcard *.c)

OBJS = $(patsubst %.c, %.o, $(SOURCES))
DEPS = $(patsubst %.c, %.d, $(SOURCES))

CFLAGS = -Werror-implicit-function-declaration -Wmissing-prototypes

all: $(OBJS)
	gcc -ggdb -o aostk $(OBJS) -lpthread `pkg-config --libs gtk+-2.0` ../libaostk.a
#size -t $(OBJS)

%.o: %.c
	@echo Compiling $< 
	@gcc `pkg-config --cflags gtk+-2.0` $(CFLAGS) -MD -c -ggdb -O0 -I .. -Wall -Wextra -o $@ $<

clean:
	-rm -f $(OBJS) $(DEPS)

-include $(DEPS)
