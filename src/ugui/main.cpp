/*
	AOS - ARM Operating System
	Copyright (C) 2007  Søren Holm (sgh@sgh.dk)

	License: wxWindows Library Licence, Version 3.1
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "ugui/Drawable.h"
#include "ugui/ugui.h"
#include "ugui/ugui_font.h"
#include "ugui/gtkfb.h"

#ifndef __arm__
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
}
#endif


extern struct aostk_font VeraMoBd;

#define PERMILLE \xE2\x80\xb0

class Cross : public Drawable {
	public:

	Cross(int x, int y, int width, int height) : Drawable(x,y,width,height) {}
	
	void draw(void) {
		/*unsigned */char buf[4] = { 	0x80+0x40+0x20 + 0x2, 0x80, 0x80+0x30, 0x00};
		ugui_line(0,0,20,20, 0xFFFF00);

// 		aostk_putstring(&VeraMoBd, 0, 0,  "ABCDEFGHIJKLMNOPQRSTUVXYZÆØÅ");
// 		aostk_putstring(&VeraMoBd, 0, 12, "abcdefghijklmnopqrstuvxyzæøå");
		aostk_putstring(&VeraMoBd, 0, 12, "ma");
// 		aostk_putstring(&VeraMoBd, 0, 24, buf);
	}
};

int main(void) {
	float angle = 0;
	int y;
	y = 100;
	UGui& gui = *UGui::instance();
	Drawable mainscreen(0 ,0, 320, 240);
	Drawable statusbar(0 ,0, 320, 30);
	Cross bladeview(0,30,320,120);
	Drawable mainfall(0,150,100,70);
	Drawable mast(105,150,110,70);
	Drawable rotation(220,150,100,70);
	Drawable quickmenu(0,220,320,20);
	

// 	Drawable menu(20 ,y-30, 200, 50);

	gtkfb_init();

	printf("sizeof Drawable: %lu\n", sizeof(Drawable));


	statusbar.setBG(0x555555);
	bladeview.setBG(0xFFFFFF);
	quickmenu.setBG(0x555555);

	mainfall.setBG(0x550000);
	mast.setBG(0x005500);
	rotation.setBG(0x000055);

// 	menu.setBG(0x005555);

	mainscreen.addChild(statusbar);
	mainscreen.addChild(bladeview);
	mainscreen.addChild(mainfall);
	mainscreen.addChild(mast);
	mainscreen.addChild(rotation);
	mainscreen.addChild(quickmenu);
	
// 	mainscreen.addChild(menu);

	gui.addRoot(mainscreen);
// 	gui.addRoot(menu);
	gui.eventLoop();

	for (;;) {
		int ch = getchar();

		switch (ch) {
			case 'A': mainfall.setBG(0xFF0000); break;
			case 'a': mainfall.setBG(0x550000); break;
			case 'B': mast.setBG(0x00FF00); break;
			case 'b': mast.setBG(0x005500); break;
			case 'C': rotation.setBG(0x0000FF); break;
			case 'c': rotation.setBG(0x000055); break;
			case 'f': rotation.focus(); break;
		}
		gui.eventLoop();
	}
}
