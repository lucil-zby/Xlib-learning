#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#define NIL (0)

int main(){
	// open display
	Display *dpy = XOpenDisplay(NULL);

	if (dpy != NULL)
	{
		// black and white color
		unsigned long blackColor = BlackPixel(dpy, DefaultScreen(dpy));
		unsigned long whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
		
		/* create window*/
		Window w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0,0,200,200,0,blackColor, whiteColor);
		
		/* map window */
		XMapWindow(dpy, w);
		
		/* flush */
		XFlush(dpy);
		
		sleep(10);
		return 0;
	} else {
		return 1;
	}
}
