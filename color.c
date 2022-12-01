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

void
hsvtorgb(int h, int s, int v, int *r, int *g, int *b)
{
	double dh, ds, dv, hh, p, q, t, ff;
	long i;

	dh = h;
	ds = s/100.0;
	dv = v/100.0;
	if(ds <= 0.0){
		*r = v;
		*g = v;
		*b = v;
	}
	hh = dh;
	if(hh >= 360.0)
		hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = dv * (1.0 - ds);
	q = dv * (1.0 - (ds * ff));
	t = dv * (1.0 - (ds * (1.0 - ff)));

	switch(i) {
		case 0:
			*r = 255 * dv;
			*g = 255 * t;
			*b = 255 * p;
			break;
		case 1:
			*r = 255 * q;
			*g = 255 * dv;
			*b = 255 * p;
			break;
		case 2:
			*r = 255 * p;
			*g = 255 * dv;
			*b = 255 * t;
			break;
		case 3:
			*r = 255 * p;
			*g = 255 * q;
			*b = 255 * dv;
			break;
		case 4:
			*r = 255 * t;
			*g = 255 * p;
			*b = 255 * dv;
			break;
		case 5:
		default:
			*r = 255 * dv;
			*g = 255 * p;
			*b = 255 * q;
			break;
	}
}

Image*
color(int x, int y, int z, int hsv)
{
	ulong n;
	Image *c;
	int i, r, g, b;

	if(hsv)
		hsvtorgb(x, y, z, &r, &g, &b);
	else{
		r = x;
		g = y;
		b = z;
	}
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

