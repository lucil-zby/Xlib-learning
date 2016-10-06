/* this is a sample file trying to create a child transparent window */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
// #include "lodepng.h"

#define NIL (0)

Display * dpy;
int screen;
Window root;
uint32_t blackColor;
uint32_t whiteColor;
XVisualInfo info;
XSetWindowAttributes attrs;

Window createWindow(Window parenet, int x, int y, int width, int height, uint32_t background);

int main(){
	dpy = XOpenDisplay(NULL);
	if (dpy != NULL)
	{	
		root = DefaultRootWindow(dpy); screen = DefaultScreen(dpy);
		blackColor = BlackPixel(dpy, screen);whiteColor = WhitePixel(dpy, screen);

		Window parent = createWindow(root, 0,0,200,200, 0);
		Window child = createWindow(parent, 0,0,100,100, whiteColor);

		XMapWindow(dpy, parent); XMapWindow(dpy, child);

		XFlush(dpy);
		XSelectInput(dpy, parent,KeyPressMask);
		XEvent ev;
		for(;;){
			XNextEvent(dpy, &ev);
			if (ev.xany.window == parent)
			{
				switch(ev.type)
				{
					case KeyPress:
						printf("%s\n", "bye");
						XCloseDisplay(dpy);
						return 0;
				}
			}
		}
	}
}

Window createWindow(Window parent, int x, int y, int width, int height, uint32_t background){
	XMatchVisualInfo(dpy, screen, 32, TrueColor, &info);
	attrs.colormap = XCreateColormap(dpy, parent, info.visual, AllocNone);
	attrs.border_pixel = 0;
	attrs.background_pixel = background;
	Window w = XCreateWindow(dpy, parent, x,y,width,height,0,info.depth,InputOutput,info.visual,CWColormap|CWBorderPixel|CWBackPixel, &attrs);
	return w;
}
