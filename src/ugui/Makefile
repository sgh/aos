APP = ../../lib$(TARGET)ugui.a

CXX = $(TARGET)g++
CC = $(TARGET)gcc
AR = $(TARGET)ar

# freetype2
FT_CFLAGS = $(shell pkg-config --cflags freetype2)
FT_LDFLAGS = $(shell pkg-config --libs freetype2)

FLAGS =  -O2 -DNDEBUG -D__STDC_LIMIT_MACROS
INCLUDES = -I ../include -I ..

ifeq ($(TARGET),arm-elf-)
FLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

CXXFLAGS = $(FLAGS) -fno-rtti -fno-exceptions -fno-default-inline
CFLAGS = $(FLAGS)

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp utf8.cpp LineDecoration.cpp TextDecoration.cpp unicode.c
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJS := $(patsubst %.c, %.o, $(OBJS))


$APP): $(OBJS) ttf2c
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

%.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

ttf2c: ttf2c.c
#	gcc  -o ttf2c -I ../include -I /usr/include/freetype2 -I .. -lfreetype ttf2c.c
	gcc  -o ttf2c $(FT_CFLAGS) $(FT_LDFLAGS) $(INCLUDES) ttf2c.c

clean:
	-rm -f $(OBJS) $(APP)
