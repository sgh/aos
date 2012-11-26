APP = ../../lib$(TOOLCHAIN_PREFIX)ugui.a

CXX = $(TOOLCHAIN_PREFIX)g++
CC = $(TOOLCHAIN_PREFIX)gcc
AR = $(TOOLCHAIN_PREFIX)ar

# freetype2
FT_CFLAGS = $(shell pkg-config --cflags freetype2)
FT_LDFLAGS = $(shell pkg-config --libs freetype2)

FLAGS =  -O2 -DNDEBUG -D__STDC_LIMIT_MACROS
INCLUDES = -I ../include -I ..

ifeq ($(TOOLCHAIN_PREFIX),arm-elf-)
FLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

ifeq ($(TOOLCHAIN_PREFIX),arm-eabi-)
FLAGS += -mcpu=arm7tdmi  -mthumb-interwork -mthumb
endif

ifeq ($(TOOLCHAIN_PREFIX),)
FLAGS += -m32
endif

CXXFLAGS = $(FLAGS) -fno-rtti -fno-exceptions -fno-default-inline
CFLAGS = $(FLAGS)

SOURCES = Drawable.cpp ugui.cpp ugui_fontrenderer.cpp utf8.cpp LineDecoration.cpp TextDecoration.cpp unicode.c
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJS := $(patsubst %.c, %.o, $(OBJS))


$APP): $(OBJS) ttf2c glyphindex render-unicode-character
	$(AR) rcs $(APP) $(OBJS)
#arm-elf-size -t $(APP)

%.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

glyphindex: glyphindex.c
	gcc -Wall -Wextra -o glyphindex $(INCLUDES) unicode.c utf8.c glyphindex.c 

ttf2c: ttf2c.c
	gcc -o ttf2c $(FT_CFLAGS) $(INCLUDES) ttf2c.c  $(FT_LDFLAGS)

render-unicode-character: render-unicode-character.c
	gcc  -o render-unicode-character $(FT_CFLAGS) $(INCLUDES) render-unicode-character.c  $(FT_LDFLAGS)

clean:
	-rm -f $(OBJS) $(APP)
