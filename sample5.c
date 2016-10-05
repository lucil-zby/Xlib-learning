#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "lodepng.h"

#define NIL (0)

Display * dpy;
Window w, root;
int defScreen;
unsigned long blackColor;
unsigned long whiteColor;
XVisualInfo info;
GC gc;
XSetWindowAttributes attrs;

char blendColor(char c1, char a1, char c2);
void blendAlpha(char* foreground, char* background, char* composited, unsigned long size);
void drawZPixmapWithAlpha(char* data, Drawable dest, int x, int y, int width, int height);
void showPNGWithAlpha(char* filename, Drawable dest, int x, int y);

int main(int argc, char** argv){
	dpy = XOpenDisplay(NULL);
	if (dpy != NULL)
	{	
		root = DefaultRootWindow(dpy); defScreen = DefaultScreen(dpy);blackColor = BlackPixel(dpy, DefaultScreen(dpy));whiteColor = WhitePixel(dpy, DefaultScreen(dpy));
		XMatchVisualInfo(dpy, defScreen, 32, TrueColor, &info);
		attrs.colormap = XCreateColormap(dpy, root,info.visual, AllocNone);
		attrs.border_pixel = 0;
		attrs.background_pixel = whiteColor;
		Window w = XCreateWindow(dpy, DefaultRootWindow(dpy), 0,0,200,200,0,info.depth,InputOutput,info.visual,CWColormap|CWBorderPixel|CWBackPixel, &attrs);
		XGCValues gcv;
		gcv.background = whiteColor;
		gcv.foreground = whiteColor;
		gc = XCreateGC(dpy,w , GCForeground | GCBackground, &gcv);
		XMapWindow(dpy, w);
		XSelectInput(dpy, w, ExposureMask | KeyPressMask);
		XFlush(dpy);
		XEvent ev;
		for(;;){
			XNextEvent(dpy, &ev);
			if (ev.xany.window == w)
			{
				switch(ev.type)
				{
					case Expose: {
						showPNGWithAlpha(argv[1], w, 0, 0);
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


char blendColor(char c1, char a1, char c2){
	/* c3 = c1*a1 + c2 * (1-a1)*/
	int ic1 = c1 >= 0 ? (int)c1 : (256 + (int) c1);
	int ia1 = a1 >= 0 ? (int)a1 : (256 + (int) a1);
	int ic2 = c2 >= 0 ? (int)c2 : (256 + (int) c2);
	int ic3 = ic1 * ia1 + ic2 * (1.0 - ((double)ia1/255.0));
	return (char) ic3;
}

void blendAlpha(char* foreground, char* background, char* composited, unsigned long size){
	unsigned long i;
	for(i = 0; i < size; i = i + 4){
		composited[i] = blendColor(foreground[i], foreground[i+3], background[i]);
		composited[i+1] = blendColor(foreground[i+1], foreground[i+3], background[i+1]);
		composited[i+2] = blendColor(foreground[i+2], foreground[i+3], background[i+2]);
		composited[i+3] = blendColor(foreground[i+3], foreground[i+3], background[i+3]);
	}
}

void drawZPixmapWithAlpha(char* data, Drawable dest, int x, int y, int width, int height){
	XImage *bkg = XGetImage(dpy, dest, 0,0 , width,height,AllPlanes, ZPixmap);
	char comp[width * height * 5];
	blendAlpha(data, bkg->data, comp, width * height * 4);
	XImage* img = XCreateImage(dpy, info.visual, info.depth, ZPixmap, 0, comp, width, height, 32, 0);
	XPutImage(dpy, dest, gc, img, 0,0,0,0,width,height);
	// XDestroyImage(img);
	XDestroyImage(bkg);
}

void showPNGWithAlpha(char* filename, Drawable dest, int x, int y){
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
		return;
	}

	free(png);

	printf("width: %d, height: %d\n", width, height);

	/* swap bits , XCreateImage takes BGR, but lodepng decodes as RBGA */
	int i = 0;
	for(;i < width * height * 4; i = i + 4)
	{
		unsigned char tmp = image[i + 2];image[i + 2] = image[i];image[i] = tmp;
	}

	drawZPixmapWithAlpha(image, dest, x, y, width, height);

	free(image);

}
