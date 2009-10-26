APP = ../../lib$(TARGET)ugui.a

CXX = $(TARGET)g++
CC = $(TARGET)gcc
AR = $(TARGET)ar

FLAGS =  -I ../include -O2 -I .. -DNDEBUG -D__STDC_LIMIT_MACROS

ifeq ($(TARGET),arm-elf-)
FLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

CXXFLAGS = $(FLAGS) -fno-rtti -fno-exceptions -fno-default-inline
CFLAGS = $(FLAGS)

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp utf8.cpp
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJS := $(patsubst %.c, %.o, $(OBJS))


$APP): $(OBJS) ttf2c
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

ttf2c: ttf2c.c
	gcc  -o ttf2c -I ../include -I /usr/include/freetype2 -I .. -lfreetype ttf2c.c

clean:
	rm $(OBJS) $(APP)
