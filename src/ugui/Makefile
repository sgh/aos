APP = ../../lib$(TARGET)ugui.a

CXX = $(TARGET)g++
AR = $(TARGET)ar

CPPFLAGS =  -fno-rtti -fno-exceptions -I ../include -Os -I .. -DNDEBUG -D__STDC_LIMIT_MACROS

ifeq ($(TARGET),arm-elf-)
CPPFLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp testfont.cpp
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

#-fno-default-inline

$APP): $(OBJS)
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

clean:
	rm $(OBJS) $(APP)
