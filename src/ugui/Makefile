APP = ../../lib$(TARGET)ugui.a

CXX = $(TARGET)g++
CC = $(TARGET)gcc
AR = $(TARGET)ar

FLAGS =  -I ../include -Os -I .. -DNDEBUG -D__STDC_LIMIT_MACROS
CPPFLAGS =  -fno-rtti -fno-exceptions $(FLAGS)
CFLAGS = $(FLAGS)

ifeq ($(TARGET),arm-elf-)
CPPFLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp utf8.cpp testfont.cpp
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJS := $(patsubst %.c, %.o, $(OBJS))

#-fno-default-inline

$APP): $(OBJS)
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

clean:
	rm $(OBJS) $(APP)
