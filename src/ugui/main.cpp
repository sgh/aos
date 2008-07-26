#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "ugui/Drawable.h"
#include "ugui/ugui.h"
#include "ugui/ugui_font.h"
#include "ugui/gtkfb.h"

class Cross : public Drawable {
	public:

	Cross(int x, int y, int width, int height) : Drawable(x,y,width,height) {}
	
	void draw(void) {
		ugui_line(0,0,20,20, 0xFFFF00);

		aostk_putstring(0,0,"ABCDEFGHIJKLMNOPQRSTUVXYZÆØÅ");
		aostk_putstring(0,12,"abcdefghijklmnopqrstuvxyzæøå");
	}
};

class SimpleLineDecoration : public DrawableDecoration {

public:
	SimpleLineDecoration() {
		_top = 10;
		_bottom = 2;
		_left = 2;
		_right = 2;
	}

#define maxX(d) ((d)->width() - 1 + _left + _right)
#define maxY(d) ((d)->height() - 1 + _top + _bottom)

	void draw(Drawable* d) {
		int col = 0x0000ff;
		int i;
		for (i=0; i<_top; i++) {
			col -= 100 / _top;
			ugui_line(_top - i, i, maxX(d), i, col);
		}

		for (i=0; i<2; i++) {
			ugui_line(0, maxY(d) - i, maxX(d), maxY(d) - i, col);
			ugui_line(i, _top, i, maxY(d), col );
			ugui_line(maxX(d) - i, _top, maxX(d) - i, maxY(d), col);
		}
	}
	
};

class SimpleLineDecoration testdecoration;

int main(void) {
	float angle = 0;
	int y;
	y = 100;
	UGui gui;
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
	gui.drawTraverseAll();

	for (;;) {
		int ch = getchar();

		switch (ch) {
			case 'A': mainfall.setBG(0xFF0000); break;
			case 'a': mainfall.setBG(0x550000); break;
			case 'B': mast.setBG(0x00FF00); break;
			case 'b': mast.setBG(0x005500); break;
			case 'C': rotation.setBG(0x0000FF); break;
			case 'c': rotation.setBG(0x000055); break;
		}
		gui.drawTraverseAll();
	}
}
