#ifndef AOSTK_H
#define AOSTK_H

#include <list.h>
#include <bits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// #include <stdio.h>

#include <aostk_context.h>
// #include <aostk_widget.h>

#define container_of(VAL,TYPE,MEMBER)  ((TYPE*)((size_t)VAL - offsetof(TYPE,MEMBER)))

#define AOSTK_UP     (1 + BIT31)
#define AOSTK_LEFT   (2 + BIT31)
#define AOSTK_DOWN   (3 + BIT31)
#define AOSTK_RIGHT  (4 + BIT31)
#define AOSTK_ENTER  (5 + BIT31)

#define LOG
// #define LOG printf("%s\n",__FUNCTION__)
#define UNUSED __attribute__((unused))

extern struct list_head aostk_widgetlist;
extern struct list_head aostk_windowlist;

struct aostk_device* current_device;


// Forward declarations
struct aostk_font;
struct aostk_widget;
//struct aostk_gc;

struct aostk_device {
	
	// Management
	void (*initialize)(void);
	void (*shutdown)(void);
	void (*redraw)(void);
	void (*clear)(void);

	// Synchronization
	void (*lock)(void);
	void (*unlock)(void);
	
	// Parameters
	unsigned int xsize;
	unsigned ysize;
	
	// Graphics primitives
	void (*putpixel)(int x, int y, uint32_t color);
	void (*line)(int x1, int y1, int x2, int y2, uint32_t color);
	void (*blockfill)(int x, int y, int width, int height, uint32_t color);
	void (*rawgrayline)(int x, int y, void* src, int n);
	
	
	// Gradient functions
	//int32_t  (*gradient_create)(/* TODO */);
	//uint32_t (*gradient_next)(/* TODO */);
};


struct aostk_point {
	unsigned int x;
	unsigned int y;
};

struct aostk_size {
	unsigned int width;
	unsigned int height;
};


struct aostk_keyaccel {
	uint32_t key;
	void (*handler)(uint32_t key);
};


void aostk_drawframe(struct aostk_widget* w, int x, int y, int width, int height, uint32_t color);

void aostk_set_device(struct aostk_device* dev);

void aostk_input(uint32_t ch);

// void aostk_redraw(struct aostk_widget* w);

void aostk_cleanup(void);

void aostk_draw(void);

void aostk_free(struct aostk_widget* w);

void aostk_key_accelerators(struct aostk_keyaccel* accels);

void keyaccel_input(uint32_t key);

static inline void aostk_lock(void) {
	if (current_device->lock)
		current_device->lock();
}

static inline void aostk_unlock(void) {
	if (current_device->unlock)
		current_device->unlock();
}

static inline void *malloc_zero(size_t size) {
	void* ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}


#endif
