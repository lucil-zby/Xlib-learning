#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "lodepng.h"

#define NIL (0)

unsigned long blackColor;
unsigned long whiteColor;

void showpng (char* filename, Display* dpy, Window w, XVisualInfo info) {
	unsigned error;
	unsigned char* image;
	unsigned width, height;
	unsigned char* png = 0;
	unsigned long pngsize;

	error = lodepng_load_file(&png, &pngsize, filename);
	if(!error) error = lodepng_decode32(&image, &width, &height, png, pngsize);
	if(error) 
	{
		printf("error %u: %s\n", error, lodepng_error_text(error));
		exit(1);
	}

	free(png);

	printf("width: %d, height: %d\n", width, height);

	/* create graphic context */
	XGCValues gcv;
	gcv.background = whiteColor;
	gcv.foreground = blackColor;
	GC gc = XCreateGC(dpy, w, GCForeground | GCBackground, &gcv);

	/* swap bits , XCreateImage takes BGR, but lodepng decodes as RBGA */
	int i = 0;
	for(;i < width * height * 4; i = i + 4)
	{
		unsigned char tmp = image[i + 2];image[i + 2] = image[i];image[i] = tmp;
	}

	XImage* img = XCreateImage(dpy, info.visual, info.depth, ZPixmap, 0, (char*) image, width, height, 32, 0);
	XPutImage(dpy, w, gc, img, 0,0,0,0,width,height);

	free(image);
}


int main(int argc, char** argv){
	Display * dpy = XOpenDisplay(NULL);
	if (dpy != NULL)
	{

		blackColor = BlackPixel(dpy, DefaultScreen(dpy));
		whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

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
						showpng(argv[1], dpy,w,info);
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

// some pngs doesn't appear normally, maybe problem is the lodepng...not sure, need clue


