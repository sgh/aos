#include <gtk/gtk.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

#include "ugui/gtkfb.h"
// struct aostk_device gtkfb_device;

#define UNUSED

#define SIZEX 320
#define SIZEY 240
#define SCALE 2


void get_sysmtime(unsigned int* u) {
}

static gboolean delete_event( UNUSED GtkWidget *widget, UNUSED GdkEvent  *event, UNUSED gpointer   data ) {
	/* If you return FALSE in the "delete_event" signal handler,
	* GTK will emit the "destroy" signal. Returning TRUE means
	* you don't want the window to be destroyed.
	* This is useful for popping up 'are you sure you want to quit?'
	* type dialogs. */

	g_print ("delete event occurred\n");

	/* Change TRUE to FALSE and the main window will be destroyed with
	* a "delete_event". */

	return FALSE;
}

/* Another callback */
static void destroy( UNUSED GtkWidget *widget, UNUSED gpointer   data ) {
	gtk_main_quit ();
}

GdkPixmap* pixmap;

static gboolean expose_event_callback (GtkWidget *widget, GdkEventExpose *event, UNUSED gpointer data) {
//   gdk_draw_arc (widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)], TRUE, 0, 0, widget->allocation.width, widget->allocation.height, 0, 64 * 360);
 
		gdk_draw_pixmap(widget->window,
		    widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);

  return FALSE;
}

static GtkWidget *drawing_area;
static GtkWidget *window;
static GtkWidget *alignment;
static 	pthread_t gtk_thread;


static void refresh(UNUSED  void* arg) {
	GdkRectangle update_rect;
	update_rect.x = 0;
	update_rect.y = 0;
	update_rect.width = SIZEX * SCALE;
	update_rect.height = SIZEY * SCALE;
// 	aostk_draw();
	gtk_widget_draw (drawing_area, &update_rect);
}

void gtkfb_init(void) {

	/* This is called in all GTK applications. Arguments are parsed
	* from the command line and are returned to the application. */
	
	g_thread_init(NULL);
 	gdk_threads_init();

	gdk_threads_enter();

	gtk_init (NULL, NULL);

	/* create a new window */
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	/* When the window is given the "delete_event" signal (this is given
	* by the window manager, usually by the "close" option, or on the
	* titlebar), we ask it to call the delete_event () function
	* as defined above. The data passed to the callback
	* function is NULL and is ignored in the callback function. */
	g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (delete_event), NULL);

	/* Here we connect the "destroy" event to a signal handler.  
	* This event occurs when we call gtk_widget_destroy() on the window,
	* or if we return FALSE in the "delete_event" callback. */
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);

	/* Sets the border width of the window. */
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);
	
	pixmap = gdk_pixmap_new(NULL, SIZEX*SCALE, SIZEY*SCALE, 24);
// 	img = gtk_image_new_from_pixmap (pixmap, NULL);
	
	drawing_area = gtk_drawing_area_new();
	
	g_signal_connect (G_OBJECT (drawing_area), "expose_event",  
                    G_CALLBACK (expose_event_callback), NULL);

  gtk_widget_set_size_request (drawing_area, SIZEX*SCALE, SIZEY*SCALE);
	
	alignment = gtk_alignment_new (0.5,0.5,0,0);
	
	gtk_container_add(GTK_CONTAINER(alignment), drawing_area);
// 	gtk_container_add(GTK_CONTAINER(alignment), img);
	
	gtk_container_add(GTK_CONTAINER(window), alignment);
	
	gtk_widget_show (drawing_area);
// 	gtk_widget_show (img);
	gtk_widget_show (alignment);
	/* and the window */
	gtk_widget_show (window);
	
	gdk_draw_rectangle(pixmap,drawing_area->style->bg_gc[GTK_WIDGET_STATE (drawing_area)], TRUE, 0, 0, SIZEX*SCALE, SIZEY*SCALE);
	
	

	/* All GTK applications must have a gtk_main(). Control ends here
	* and waits for an event to occur (like a key press or
	* mouse event). */
	
	 
   /* Create independent threads each of which will execute function */

	
	g_timeout_add (100, &refresh, NULL);

	pthread_create( &gtk_thread, NULL, gtk_main, NULL);
}

static int bound_x1;
static int bound_x2;
static int bound_y1;
static int bound_y2;

void ugui_lock(void) {
}

void ugui_unlock(void) {
}

void ugui_line(int x1, int y1, int x2, int y2, unsigned int color) {
	int dx,dy;
	GdkColormap *cmap;
	GdkColor acolor;
// 	GdkRectangle rect = {
// 		.x = 0,
// 		.y = 0,
// 		.width = 100,
// 		.height = 100,
// 	};

	x1 += bound_x1;
	x2 += bound_x1;
	y1 += bound_y1;
	y2 += bound_y1;

	gdk_threads_enter();
	
	cmap = gdk_colormap_get_system();
	
  acolor.red   = ((color >> 16) & 0xFF) * 0x101;
  acolor.green = ((color >>  8) & 0xFF) * 0x101;
  acolor.blue  = ((color >>  0) & 0xFF) * 0x101;

  if (!gdk_color_alloc(cmap, &acolor)) {
    g_error("couldn't allocate color");
		exit(1);
  }
	
	gdk_gc_set_rgb_fg_color(drawing_area->style->fg_gc[GTK_WIDGET_STATE (drawing_area)], &acolor );

	for (dx = 0; dx<SCALE; dx++)
	for (dy = 0; dy<SCALE; dy++)
		gdk_draw_line(pixmap, drawing_area->style->fg_gc[GTK_WIDGET_STATE (drawing_area)], x1*SCALE+dx, y1*SCALE+dy, x2*SCALE+dx, y2*SCALE+dy);

	gdk_threads_leave();
}

void ugui_set_bounds(int x1, int y1, int x2, int y2) {
	bound_x1 = x1;
	bound_x2 = x2;
	bound_y1 = y1;
	bound_y2 = y2;
}

void ugui_fill(int x, int y, int width, int height, unsigned int color) {
	int i;
 	for (i = 0; i<height ; i++)
		ugui_line(x, y + i, x + width - 1, y  + i, color);
}

void ugui_putpixel(int x, int y, unsigned int color) {
	int dx,dy;
	GdkColor rgb;

	x += bound_x1;
	y += bound_y1;

	if (x > bound_x2 || y > bound_y2)
		return;

	gdk_threads_enter();
	rgb.red   = ((color >> 16) & 0xFF) * 0x101;
  rgb.green = ((color >>  8) & 0xFF) * 0x101;
  rgb.blue  = ((color >>  0) & 0xFF) * 0x101;


	gdk_gc_set_rgb_fg_color(drawing_area->style->fg_gc[GTK_WIDGET_STATE (drawing_area)], &rgb );

	for (dx = 0; dx<SCALE; dx++)
	for (dy = 0; dy<SCALE; dy++)
		gdk_draw_point(pixmap, drawing_area->style->fg_gc[GTK_WIDGET_STATE (drawing_area)], x*SCALE + dx, y*SCALE + dy);
	 
	gdk_threads_leave();
}

static void gtkfb_shutdown(void) {
	pthread_join( gtk_thread, NULL);
	gdk_threads_leave();
}

static void gtkfb_redraw(void) {
}

	// struct aostk_device gtkfb_device = {
	// 	.xsize = SIZEX * SCALE,
	// 	.ysize = SIZEY * SCALE,
	// 	.initialize = &gtkfb_init,
	// 	.shutdown = &gtkfb_shutdown,
	// 	.line = &gtkfb_line,
	// 	.putpixel = &gtkfb_putpixel,
	// 	.redraw = &gtkfb_redraw,
	// };


/*int main( int   argc, char *argv[] ) { 
	
	gtkfb_device.initialize();
	
	gtkfb_device.line(0,0,100,100,0);
	gtkfb_device.putpixel(50,100,0);

	gtkfb_device.shutdown();
	
	return 0;
}*/
