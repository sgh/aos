#include <ugui/ugui.h>
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
}

void ugui_set_bounds(int x1, int y1, int x2, int y2) {
}

void ugui_line(int x1, int y1, int x2, int y2, unsigned int color) {
	rfbDrawLine(server, x1, y1, x2, y2, color);
}

}

Drawable* td;

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
	td = new Drawable(0,0,100,100);
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
