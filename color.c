#include "a.h"

typedef struct Color Color;

struct Color
{
	ulong n;
	Image *i;
};

Color cache[255] = {0};
int ncolors = 0;

Image*
ealloccolor(ulong n)
{
	Image *i;

	i = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, n);
	if(i == nil)
		sysfatal("allocimage: %r");
	return i;
}

Image*
color(int r, int g, int b)
{
	ulong n;
	Image *c;
	int i;

	c = nil;
	n = (r << 24) | (g << 16) | (b << 8) | 0xFF;
	for(i = 0; i < nelem(cache); i++){
		if(cache[i].n == n){
			c = cache[i].i;
			break;
		}
	}
	if(c == nil){
		c = ealloccolor(n);
		cache[ncolors].n = n;
		cache[ncolors].i = c;
		ncolors++;
		if(ncolors == nelem(cache))
			ncolors = 0;
	}
	return c;
}

Image*
grayscale(int n)
{
	return color(n, n, n);
}

