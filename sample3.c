#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NIL (0)

void paintbox(Display * dpy, Window w, int x, int y, int width, int height, int borderWidth, unsigned long fillColor, unsigned long borderColor){
	XGCValues gcv;
	gcv.foreground = fillColor;
	GC gc = XCreateGC(dpy, w, GCForeground, &gcv);
	XFillRectangle(dpy, w, gc, x, y, width, height);
	gcv.foreground = borderColor;
	gcv.line_width = borderWidth;
	XChangeGC(dpy, gc, GCLineWidth | GCForeground, &gcv);
	XSetForeground(dpy, gc, borderColor);
	XDrawRectangle(dpy, w, gc, x, y, width, height);
}

char* getRandColor(){
	srand (time(NULL));
	char* color = (char*) malloc(13 * sizeof(char));
	memcpy(color, "rgb:", sizeof("rgb:"));
	int r = rand() % 256, g = rand() % 256,	b = rand() % 256;
	sprintf(color+4,"%02x/%02x/%02x", r,g,b);
	printf("%s\n",color);
	return color;
}

int main(){
	Display * dpy = XOpenDisplay(NULL);
	if (dpy != NULL)
	{

		unsigned long blackColor = BlackPixel(dpy, DefaultScreen(dpy));
		unsigned long whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

		XVisualInfo info;
		/*  get the visual info */
		XMatchVisualInfo(dpy, DefaultScreen(dpy), 32, TrueColor, &info);
		XSetWindowAttributes attrs;
		/* set attributes */
		attrs.colormap = XCreateColormap(dpy, DefaultRootWindow(dpy), info.visual, AllocNone);
		attrs.border_pixel = blackColor;
		attrs.background_pixel = whiteColor;

		/* create a window */
		Window w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0,0,200,200,0,info.depth,InputOutput,info.visual,CWColormap|CWBorderPixel|CWBackPixel, &attrs);

		/* map window */

		XMapWindow(dpy, w);

		/* select which event to respond to */
		XSelectInput(dpy, w, ExposureMask | KeyPressMask);

		/* flush */

		XFlush(dpy);

		XEvent ev;


		/* get random color */

		char* c1 = getRandColor();char* c2 = getRandColor();
		XColor cr1,cr2;
		XParseColor(dpy, attrs.colormap, c1, &cr1);	XParseColor(dpy, attrs.colormap, c2, &cr2);

		for(;;){
			/* for multiple windows, use XWindowEvent */
			XNextEvent(dpy, &ev);
			if (ev.xany.window == w)
			{
				switch(ev.type)
				{
					case Expose: {
						paintbox(dpy, w, 10,10,60,60,10, cr1.pixel, cr2.pixel);
						free(c1);free(c2);
						break;
					}
					case KeyPress:
						printf("%s\n", "bye");
						XDestroyWindow(dpy, w);
						XCloseDisplay(dpy);
						return 0;
						break;
				}
			}
		}
	}
}
