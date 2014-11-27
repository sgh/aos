/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#include <ugui/ugui.h>
#include <ugui/ugui_font.h>
#include <ugui/Drawable.h>
#include <rfb/rfb.h>


rfbScreenInfoPtr server;

extern "C" {
void sem_init(semaphore_t* sem, uint32_t count) {
}
void sem_up(semaphore_t* sem) {
}
void sem_down(semaphore_t* sem) {
}
uint8_t sem_timeout_down(semaphore_t* sem, uint32_t timeout) {
}
void mutex_init(mutex_t* m) {
}
void mutex_lock(mutex_t* m) {
}
void mutex_unlock(mutex_t* m) {
}

void get_sysmtime(uint32_t* time) {
	*time = 0;
}

void ugui_fill(int x, int y, int width, int height, unsigned int color) {
	rfbFillRect(server, x, y, x+width, y+height, color);
}

void ugui_putpixel16_native(uint16_t bitmap, int x, int y, unsigned char color) {
	if (bitmap & 0x8000) rfbDrawPixel(server, x+0, y, color);
	if (bitmap & 0x4000) rfbDrawPixel(server, x+1, y, color);
	if (bitmap & 0x2000) rfbDrawPixel(server, x+2, y, color);
	if (bitmap & 0x1000) rfbDrawPixel(server, x+3, y, color);
	if (bitmap & 0x0800) rfbDrawPixel(server, x+4, y, color);
	if (bitmap & 0x0400) rfbDrawPixel(server, x+5, y, color);
	if (bitmap & 0x0200) rfbDrawPixel(server, x+6, y, color);
	if (bitmap & 0x0100) rfbDrawPixel(server, x+7, y, color);
	if (bitmap & 0x0080) rfbDrawPixel(server, x+8, y, color);
	if (bitmap & 0x0040) rfbDrawPixel(server, x+9, y, color);
	if (bitmap & 0x0020) rfbDrawPixel(server, x+10, y, color);
	if (bitmap & 0x0010) rfbDrawPixel(server, x+11, y, color);
	if (bitmap & 0x0008) rfbDrawPixel(server, x+12, y, color);
	if (bitmap & 0x0004) rfbDrawPixel(server, x+13, y, color);
	if (bitmap & 0x0002) rfbDrawPixel(server, x+14, y, color);
	if (bitmap & 0x0001) rfbDrawPixel(server, x+15, y, color);
	
}

void ugui_set_bounds(int x1, int y1, int x2, int y2) {
}

void ugui_line(int x1, int y1, int x2, int y2, unsigned int color) {
	rfbDrawLine(server, x1, y1, x2, y2, color);
}

unsigned int ugui_alloc_color(unsigned int rgb) {
        return rgb;
}


}

extern const struct aostk_font VeraMoBd;

class MyDrawable : public Drawable {
	public:
		MyDrawable(int x,int y,int width,int height) : Drawable(x,y,width,height) {}
		void draw() {
			_ctx.text_color = 0xFF00FF ;
			aostk_putstring(&VeraMoBd, 0, 0, "TestHest");
		}
};


MyDrawable* td;

void keyevent(rfbBool down, rfbKeySym key, struct _rfbClientRec* cl) {
	printf("%d\n", key);
	if (down) {
		switch (key) {
			case 'a': td->setBG(0xFF0000); break;
			case 's': td->setBG(0x00FF00); break;
			case 'd': td->setBG(0x0000FF); break;
		}
	}
}


int main(int argc,char** argv)
{
	td = new MyDrawable(0,0,100,100);
  server = rfbGetScreen(&argc,argv,400,300,8,3,4);
  server->frameBuffer = (char*)malloc(400*300*4);
	server->kbdAddEvent = keyevent;
	server->serverFormat.redShift   = 16;
	server->serverFormat.greenShift = 8;
	server->serverFormat.blueShift  = 0;
  rfbInitServer(server);
  rfbRunEventLoop(server,-1,TRUE);
	
	td->setBG(0xFFFF00);
	
	UGui::instance()->addRoot(*td);
	
	while( 1 ) {
		bool updated = UGui::instance()->eventLoop();
		if (updated) {
			printf(".");
			fflush(0);
			rfbMarkRectAsModified(server, 0, 0, 400, 300);
		}
		usleep(10000);
	}
	
  return(0);
}
