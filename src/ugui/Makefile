APP = ../../libugui.a

CXX = arm-elf-g++
AR = arm-elf-ar

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp
OBJS = $(patsubst %.cpp, %.o, $(SOURCES))

CPPFLAGS =  -fno-rtti -fno-exceptions -mcpu=arm7tdmi -I ../include -mthumb-interwork -mthumb -Os -I .. -DNDEBUG
#-fno-default-inline

all: $(OBJS)
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

clean:
	rm $(OBJS)
