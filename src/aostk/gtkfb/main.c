// #include <stdio.h>
#include <string.h>
#include <list.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>
#include <stdio.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


// #include <mutex.h>
#include <aostk.h>
#include <aostk_widget.h>
#include <aostk_label.h>
#include <aostk_vbox.h>
#include <aostk_hbox.h>
#include <aostk_window.h>
#include <aostk_button.h>
#include <aostk_checkbox.h>
#include <aostk_listview.h>
#include <aostk_spinbox.h>
#include <aostk_fontrenderer.h>

// #include <termios.h>

// #define  xres  /*3*/80
// #define  yres  /*2*/30
#define xres 80
#define yres 30

// unsigned char fbdata[xres*yres];

// unsigned char* framebuffer;


#define ABS(val) ((val)>0 ? (val) : -(val))

static struct aostk_device textfbdev;

unsigned char framebuffer[xres*yres];

static void textfb_redraw(void) {
	static int count = 1;
	unsigned int x,y;

	printf("%d:\n+",count++);
	for (y=0; y<xres; y++)
		printf("-");
	printf("+\n");
	
	for (y=0; y<yres; y++) {
		printf("|");
		for (x=0; x<xres; x++) {
			if (framebuffer[y*xres + x])
				printf("%X",framebuffer[y*xres + x]);
			else
				printf(" ");
// 			printf("%c", framebuffer[y*xres + x] ? '#' : ' ');
		}
		printf("|");
		printf("\n");
	}

	printf("+");
	for (y=0; y<xres; y++)
		printf("-");
	printf("+\n");
	
}

static void textfb_clear(void) {
	memset(framebuffer, 0x0, sizeof(framebuffer));
}

static void textfb_initialize(void) {
	textfb_clear();
}

static void textfb_putpixel(int x, int y, unsigned int color) {
	framebuffer[y*xres + x] = color;
// 	printfb();
}


static void textfb_line(int x1, int y1, int x2, int y2, unsigned int color) {
	float fx, fy;
	int x, y;
	int i;
	float offx, offy;
	float dx,dy;
	int count;

	count = ABS(x2-x1) > ABS(y2-y1) ? ABS(x2-x1) : ABS(y2-y1);

// 	printf("%d steps\n",count);

	dx = (float)(x2-x1)/count;
	dy = (float)(y2-y1)/count;

// 	printf("dx: %.3f\n", dx);
// 	printf("dy: %.3f\n", dy);
	
	for (i=0; i<count+1; i++) {
		int col = color;
		float factor;
		
		fx = x1 + dx*i;
		fy = y1 + dy*i;

		x = fx;
		y = fy;

		offy = y - fy;
		offx = x - fx;

		factor = 0;

		/** @todo determine the current direction and graduate the line ortogornally of that direction */
#if ANTIALIAS
		if (offy < 0) {
			factor = (ABS(offy));
			aostk_putpixel(x,y+1, col * factor);
		}

		if (offy > 0) {
			factor =  (1 - ABS(offy));
			aostk_putpixel(x,y-1, col * factor);
		}

		if (offx < 0) {
			factor =  (ABS(offx));
			aostk_putpixel(x+1,y, col * factor);
		}

		if (offy > 0) {
			factor =  (1 - ABS(offx));
			aostk_putpixel(x-1,y, col * factor);
		}
#endif
		
		factor = 1 - factor;

		aostk_putpixel(x,y, col * factor);

// 		printf("%.1f %.1f %.1f\n", fx,fy,off);
	}
// 	textfb_clear();
}


UNUSED static struct aostk_device textfbdev = {
	.initialize = textfb_initialize,
	.redraw = textfb_redraw,
	.line = textfb_line,
	.putpixel = textfb_putpixel,
	.clear = textfb_clear,
};

struct aostk_widget* check;


static void onactivated(UNUSED struct aostk_button* button, void* arg) {
// 	LOG;
// 	aostk_activate(arg);
	printf("%s\n", arg);
}



struct aostk_device gtkfb_device;


static void lv_onactivated(UNUSED struct aostk_listview* l, void* arg) {
	aostk_activate(arg);
}

int main() {
// 	struct aostk_widget* w;
// 	struct aostk_widget* w2;
// 	struct aostk_widget* vbox;
	struct aostk_widget* hbox;
	struct aostk_widget* button;
	struct aostk_widget* window;
// 	struct aostk_widget* check;
	struct aostk_widget* spinbox;
	struct aostk_widget* listview;
	struct aostk_widget* label;
	int i;

#ifndef WIN32
	struct termios settings,old_settings;
	
	tcgetattr(0,&old_settings);
	tcgetattr(0, &settings);
	settings.c_lflag &= ~ICANON;
	settings. c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &settings);
	setbuf(stdin, NULL);
#endif

	atexit(aostk_cleanup);

// 	aostk_set_device(&textfbdev);
 	aostk_set_device(&gtkfb_device);


	window = aostk_window_create(10, 3, 160, 140);

// 	listview = aostk_listview_create(10,3,window);

// 	aostk_listview_onactivated(listview, &lv_onactivated, NULL);


// 	for (i=0 ; i<3; i++) {
// 		hbox = aostk_hbox_create(10, 10, NULL);
// 		button = aostk_button_create(/*&verafont,*/ "Value");
// 		spinbox = aostk_spinbox_create(0);
// 		aostk_hbox_push_start(hbox, button);
// 		aostk_hbox_push_start(hbox, spinbox);
// 		aostk_listview_push_start(listview, hbox, spinbox);
// 		aostk_button_onactivated(button, &onactivated, spinbox);
// 	}

	label = aostk_label_create("Are you sure?");
	hbox = aostk_hbox_create(10, 10, window);
	button = aostk_button_create("Yes");
	aostk_button_onactivated(button, &onactivated, "Yes");
	aostk_hbox_push_start(hbox, button);
	button = aostk_button_create("No");
	aostk_button_onactivated(button, &onactivated, "No");
	aostk_hbox_push_start(hbox, button);

	aostk_widget_reparent(label,window);


	for (;;) {
		int c;
// 		fd_set rfds;
// 		struct timeval tv;
// 		int retval;
// 
// 		/* Watch stdin (fd 0) to see when it has input. */
// 		FD_ZERO(&rfds);
// 		FD_SET(0, &rfds);
// 
// 		/* Wait up to five seconds. */
// 		tv.tv_sec = 0;
// 		tv.tv_usec = 100000;
// 
// 		retval = select(1, &rfds, NULL, NULL, &tv);
// 		/* Don't rely on the value of tv now! */


// 		if (retval == 0)
// 			printf("%d\n",retval);

// 		if (retval) {
			c = getchar();
			if (c == 'q') {
				printf("Quit\n");
				break;
			}

			switch (c) {
				case 'w' : printf("Up\n");       aostk_input(AOSTK_UP);    break;
				case 'a' : printf("Left\n");     aostk_input(AOSTK_LEFT);  break;
				case 's' : printf("Down\n");     aostk_input(AOSTK_DOWN);  break;
				case 'd' : printf("Right\n");    aostk_input(AOSTK_RIGHT); break;
				case 10  : printf("Activate\n"); aostk_input(AOSTK_ENTER); break;
				case  9  : printf("Next\n");     aostk_focus_next();       break;
				default  : printf("No action (%d)\n", c); break;
			}
// 		}
// 		usleep(1000000);
// 		pthread_yield();
	}

// 	w = aostk_widget_create(5,5,5,5, w2);

// 	aostk_redraw();
// 	printfb();

	tcsetattr(0, TCSANOW, &old_settings);
	return 0;
	
}
