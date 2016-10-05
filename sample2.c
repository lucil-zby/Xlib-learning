#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define NIL (0)

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

		for(;;){
			/* for multiple windows, use XWindowEvent */
			XNextEvent(dpy, &ev);
			if (ev.xany.window == w)
			{
				switch(ev.type)
				{
					case Expose: {
						/* fill a black rectangle of 100x100 at (10,10) */
						GC gc = XCreateGC(dpy, w, 0, NIL);
						XSetForeground(dpy, gc, blackColor);
						XFillRectangle(dpy, w, gc, 10, 10, 100, 100);
						/* xflush unnecessory, as XNextEvent do the flush */
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
