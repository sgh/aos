#include <stdio.h>
#include <string.h>
#include <list.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

#include <aostk.h>
#include <aostk_widget.h>
#include <aostk_label.h>
#include <aostk_vbox.h>
#include <aostk_window.h>
#include <aostk_button.h>
#include <aostk_checkbox.h>
#include <aostk_fontrenderer.h>
#include <aostk_context.h>


#define  xres  /*3*/80
#define  yres  /*2*/30

LIST_HEAD(aostk_widgetlist);
LIST_HEAD(aostk_windowlist);


static void aostk_free_traverse(struct list_head* l) {
	struct list_head* it;
	struct aostk_widget* w;
	list_for_each(it, l) {
		w = child_to_widget(it);
		aostk_free_traverse(&w->childlist);
// 		printf("Deleting child %s\n", w->name);
		list_erase(&w->childnode);  // Remove from parent-widgets list
		list_erase(&w->widgetnode); // Remove from global widget-list
		free(w);
	}
}

void aostk_free(struct aostk_widget* w) {
	if (!w) {
// 		printf("Deleting all widgets\n");
		aostk_free_traverse(&aostk_windowlist);
		return;
	}
	aostk_free_traverse(&w->childlist);
	
	list_erase(&w->childnode);  // Remove from parent-widgets list
	list_erase(&w->widgetnode); // Remove from global widget-list

// 	printf("Deleting parent %s\n", w->name);
	free(w);
}

#define ABS(val) ((val)>0 ? (val) : -(val))
extern int yoffset;
void aostk_draw(void) {
	struct list_head* it;
	struct aostk_widget* w;

	
	yoffset++;
	if (yoffset >= 25)
		yoffset = 0;

	list_for_each(it, &aostk_windowlist) {
		w = (struct aostk_widget*)child_to_widget(it);
		w->draw(w);
	}

// 	current_device->redraw();
}


void aostk_cleanup(void) {
	aostk_free(NULL);
}



void aostk_set_device(struct aostk_device* dev) {
	current_device = dev;
// 	if (current_device->initialize)
// 		current_device->initialize();
}

/** @todo rename to blockfill
void aostk_drawbox(struct aostk_gc* gc, int x, int y, int width, int height) {
	LOG;
	width--;
// 	printf("drawbox %d %d %d %d\n",x,y,width,height);
	while (height--) {
		aostk_line(x, y, x + width, y, gc->fg_col);
		y++;
	}
}
*/

void aostk_input(uint32_t c) {
	uint8_t key_handled = 0;

	if (active_widget && active_widget->input)
		key_handled = active_widget->input(active_widget, c);

	keyaccel_input(c);
}


