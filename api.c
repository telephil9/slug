#include "a.h"

typedef struct Tmatrix Tmatrix;
typedef struct Style Style;

struct Tmatrix
{
	Point origin;
	double angle;
};

struct Style
{
	int		nostroke;
	int		strokecap;
	Image	*stroke;
	int		strokeweight;
	int		nofill;
	Image	*fill;
};

enum
{
	Crgb,
	Chsv,
};

int		looping;
int		framerate;
Image	*canvas;
int		width;
int		height;
int		colormode;
int		nostroke;
int		strokecap;
Image	*stroke;
int		strokeweight;
int		nofill;
Image	*fill;
Point	origin;
double	angle;

Tmatrix tstack[255] = {0};
int ntstack;
Style sstack[255] = {0};
int nsstack;

void
initstate(lua_State *L)
{
	Rectangle r;

	ntstack = 0;
	looping = 1;
	framerate = 90;
	width = 500;
	height = 500;
	r = Rect(0, 0, width, height);
	canvas = allocimage(display, r, screen->chan, 0, DWhite);
	colormode = Crgb;
	nostroke = 0;
	strokecap = Endsquare;
	stroke = display->black;
	strokeweight = 0;
	nofill = 0;
	fill = display->white;
	origin = ZP;
	angle = 0.0;
	lset(L, "mouseX", 0);
	lset(L, "mouseY", 0);
	lpushpixels(L);
}

void
reset(void)
{
	origin = ZP;
	angle = 0.0;
}

void
drawcanvas(void)
{
	draw(screen, screen->r, canvas, nil, ZP);
	flushimage(display, 1);
}

Point
canvaspt(int x, int y)
{
	Point p;

	p = Pt(x, y);
	if(fabs(angle) > 0.000001){
		p.x = x * cos(angle) - y * sin(angle);
		p.y = x * sin(angle) + y * cos(angle);
	}
	p = addpt(p, origin);
	return p;
}

int
cnoloop(lua_State*)
{
	looping = 0;
	return 0;
}

int
cloop(lua_State*)
{
	looping = 1;
	return 0;
}

int
cframerate(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	if(n < 0)
		return luaL_argerror(L, 1, "framerate should be greater than 0");
	framerate = n;
	return 0;
}

int
csize(lua_State *L)
{
	int w, h;

	w = luaL_checkinteger(L, 1);
	h = luaL_checkinteger(L, 2);
	if(drawing)
		return 0;
	width = w;
	height = h;
	freeimage(canvas);
	canvas = allocimage(display, Rect(0, 0, w, h), screen->chan, 0, DNofill);
	lpushpixels(L);
	resize(L, w, h);
	return 0;
}

void
checkcolor(lua_State *L, int index, int *r, int *g, int *b)
{
	lua_pushstring(L, "r");
	lua_gettable(L, index);
	*r = luaL_checkinteger(L, -1);
	lua_pushstring(L, "g");
	lua_gettable(L, index);
	*g = luaL_checkinteger(L, -1);
	lua_pushstring(L, "b");
	lua_gettable(L, index);
	*b = luaL_checkinteger(L, -1);
}

void
pushcolor(lua_State *L, int r, int g, int b)
{
	lua_newtable(L);
	lua_pushinteger(L, r);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L, g);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L, b);
	lua_setfield(L, -2, "b");
}

Image*
getcolor(lua_State *L)
{
	Image *i;
	int c, r, g, b;

	c = lua_gettop(L);
	if(c == 1){
		if(lua_istable(L, 1))
			checkcolor(L, 1, &r, &g, &b);
		else
			r = g = b = luaL_checkinteger(L, 1);
		i = color(r, g, b, 0);
	}else if(c == 3){
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
		i = color(r, g, b, colormode == Chsv);
	}else{
		luaL_error(L, "invalid argument count (expected 1 or 3 but received %d)", c);
		return nil;
	}
	return i;
}

int
ccolormode(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	if(n != Crgb && n != Chsv)
		return luaL_argerror(L, 1, "expected RGB or HSV");
	colormode = n;
	return 0;
}

int
cbackground(lua_State *L)
{
	Image *i;

	i = getcolor(L);
	draw(canvas, canvas->r, i, nil, ZP);
	return 0;
}

int
cnostroke(lua_State*)
{
	nostroke = 1;
	return 0;
}

int
cstrokecap(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	if(n == Endsquare || n == Enddisc)
		strokecap = n;
	return 0;
}

int
cstroke(lua_State *L)
{
	Image *i;

	i = getcolor(L);
	stroke = i;
	nostroke = 0;
	return 0;
}

int
cnofill(lua_State*)
{
	nofill = 1;
	return 0;
}

int
cfill(lua_State *L)
{
	Image *i;

	i = getcolor(L);
	fill = i;
	nofill = 0;
	return 0;
}


int
cstrokeweight(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	strokeweight = n/2;
	return 0;
}

int
cpoint(lua_State *L)
{
	Point p1, p2;
	int x, y;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	p1 = canvaspt(x, y);
	p2 = canvaspt(x + strokeweight, y + strokeweight);
	if(!nostroke)
		if(strokecap == Endsquare)
			draw(canvas, Rpt(p1, p2), stroke, nil, ZP);
		else if(strokecap == Enddisc)
			fillellipse(canvas, p1, strokeweight/2.0, strokeweight/2.0, stroke, ZP);
	return 0;
}

int
cline(lua_State *L)
{
	Point p1, p2;
	int x1, x2, y1, y2;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	p1 = canvaspt(x1, y1);
	p2 = canvaspt(x2, y2);
	if(!nostroke)
		line(canvas, p1, p2, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
csquare(lua_State *L)
{
	Point p[5];
	int x, y, w;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	w = luaL_checkinteger(L, 3);
	p[0] = canvaspt(x, y);
	p[1] = canvaspt(x + w, y);
	p[2] = canvaspt(x + w, y + w);
	p[3] = canvaspt(x, y+w);
	p[4] = p[0];
	if(!nofill)
		fillpoly(canvas, p, 4, 0, fill, ZP);
	if(!nostroke)
		poly(canvas, p, 5, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
crect(lua_State *L)
{
	Point p[5];
	int x, y, w, h;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	w = luaL_checkinteger(L, 3);
	h = luaL_checkinteger(L, 4);
	p[0] = canvaspt(x, y);
	p[1] = canvaspt(x + w, y);
	p[2] = canvaspt(x + w, y + h);
	p[3] = canvaspt(x, y + h);
	p[4] = p[0];
	if(!nofill)
		fillpoly(canvas, p, 4, 0, fill, ZP);
	if(!nostroke)
		poly(canvas, p, 5, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
ccircle(lua_State *L)
{
	Point p;
	int x, y, a;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	a = luaL_checkinteger(L, 3) / 2;
	p = canvaspt(x, y);
	if(!nofill)
		fillellipse(canvas, p, a, a, fill, ZP);
	if(!nostroke)
		ellipse(canvas, p, a, a, strokeweight, stroke, ZP);
	return 0;
}

int
cellipse(lua_State *L)
{
	Point p;
	int x, y, a, b;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	a = luaL_checkinteger(L, 3) / 2;
	b = luaL_checkinteger(L, 4) / 2;
	p = canvaspt(x, y);
	if(!nofill)
		fillellipse(canvas, p, a, b, fill, ZP);
	if(!nostroke)
		ellipse(canvas, p, a, b, strokeweight, stroke, ZP);
	return 0;
}

int
carc(lua_State *L)
{
	Point p;
	int x, y, a, b, c, d;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	a = luaL_checkinteger(L, 3);
	b = luaL_checkinteger(L, 4);
	c = luaL_checknumber(L, 5) * 180.0/PI;
	d = luaL_checknumber(L, 6) * 180.0/PI;
	p = canvaspt(x, y);
	if(!nofill)
		fillarc(canvas, p, a, b, fill, ZP, c, d);
	if(!nostroke)
		arc(canvas, p, a, b, strokeweight, stroke, ZP, c, d);
	return 0;
}

int
cbezier(lua_State *L)
{
	Point p0, p1, p2, p3;
	int x1, y1, x2, y2, x3, y3, x4, y4;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	x3 = luaL_checkinteger(L, 5);
	y3 = luaL_checkinteger(L, 6);
	x4 = luaL_checkinteger(L, 7);
	y4 = luaL_checkinteger(L, 8);
	p0 = canvaspt(x1, y1);
	p1 = canvaspt(x2, y2);
	p2 = canvaspt(x3, y3);
	p3 = canvaspt(x4, y4);
	if(!nostroke)
		bezier(canvas, p0, p1, p2, p3, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
ctriangle(lua_State *L)
{
	Point p[4];
	int x1, y1, x2, y2, x3, y3;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	x3 = luaL_checkinteger(L, 5);
	y3 = luaL_checkinteger(L, 6);
	p[0] = canvaspt(x1, y1);
	p[1] = canvaspt(x2, y2);
	p[2] = canvaspt(x3, y3);
	p[3] = p[0];
	if(!nofill)
		fillpoly(canvas, p, 3, 0, fill, ZP);
	if(!nostroke)
		poly(canvas, p, 4, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
cquad(lua_State *L)
{
	Point p[5];
	int x1, y1, x2, y2, x3, y3, x4, y4;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	x3 = luaL_checkinteger(L, 5);
	y3 = luaL_checkinteger(L, 6);
	x4 = luaL_checkinteger(L, 7);
	y4 = luaL_checkinteger(L, 8);
	p[0] = canvaspt(x1, y1);
	p[1] = canvaspt(x2, y2);
	p[2] = canvaspt(x3, y3);
	p[3] = canvaspt(x4, y4);
	p[4] = p[0];
	if(!nofill)
		fillpoly(canvas, p, 4, 0, fill, ZP);
	if(!nostroke)
		poly(canvas, p, 5, strokecap, strokecap, strokeweight, stroke, ZP);
	return 0;
}

int
ctranslate(lua_State *L)
{
	int x, y;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	origin = addpt(origin, Pt(x, y));
	return 0;
}

int
crotate(lua_State *L)
{
	angle += luaL_checknumber(L, 1);
	return 0;
}

int
cradians(lua_State *L)
{
	int a;

	a = luaL_checkinteger(L, 1);
	lua_pushnumber(L, a*PI/180.0);
	return 1;
}

int
cpushmatrix(lua_State *L)
{
	if(ntstack == nelem(tstack) - 1)
		return luaL_error(L, "stack overflow");
	tstack[ntstack].origin = origin;
	tstack[ntstack].angle  = angle;
	ntstack++;
	return 0;
}

int
cpopmatrix(lua_State *L)
{
	if(ntstack == 0)
		return luaL_error(L, "stack underflow");
	ntstack--;
	origin = tstack[ntstack].origin;
	angle  = tstack[ntstack].angle;
	return 0;
}

int
cpushstyle(lua_State *L)
{
	if(nsstack == nelem(sstack) - 1)
		return luaL_error(L, "stack overflow");
	sstack[nsstack].nostroke = nostroke;
	sstack[nsstack].strokecap = strokecap;
	sstack[nsstack].stroke = stroke;
	sstack[nsstack].strokeweight = strokeweight;
	sstack[nsstack].nofill = nofill;
	sstack[nsstack].fill = fill;
	nsstack++;
	return 0;
}

int
cpopstyle(lua_State *L)
{
	if(nsstack == 0)
		return luaL_error(L, "stack underflow");
	nsstack--;
	nostroke = sstack[nsstack].nostroke;
	strokecap = sstack[nsstack].strokecap;
	stroke = sstack[nsstack].stroke;
	strokeweight = sstack[nsstack].strokeweight;
	nofill = sstack[nsstack].nofill;
	fill = sstack[nsstack].fill;
	return 0;
}

int
cpush(lua_State *L)
{
	cpushmatrix(L);
	cpushstyle(L);
	return 0;
}

int
cpop(lua_State *L)
{
	cpopmatrix(L);
	cpopstyle(L);
	return 0;
}

int
ccolor(lua_State *L)
{
	int r, g, b;

	r = luaL_checkinteger(L, 1);
	g = luaL_checkinteger(L, 2);
	b = luaL_checkinteger(L, 3);
	if(colormode == Chsv)
		hsvtorgb(r, g, b, &r, &g, &b);
	pushcolor(L, r, g, b);
	return 1;
}

int
clerpcolor(lua_State *L)
{
	int r0, g0, b0, r1, g1, b1, r, g, b;
	double f;

	checkcolor(L, 1, &r0, &g0, &b0);
	checkcolor(L, 2, &r1, &g1, &b1);
	f = luaL_checknumber(L, 3);
	if(f < 0.0)
		f = 0.0;
	else if(f > 1.0)
		f = 1.0;
	r = f * (r1 - r0) + r0;
	g = f * (g1 - g0) + g0;
	b = f * (b1 - b0) + b0;
	pushcolor(L, r, g, b);
	return 1;
}

int
cloadpixels(lua_State *L)
{
	char ebuf[ERRMAX];

	if(unloadimage(canvas, canvas->r, pixels->data, pixels->ndata) < 0){
		errstr(ebuf, sizeof ebuf);
		return luaL_error(L, "unable to load pixels: %s", ebuf);
	}
	return 0;
}

int
cupdatepixels(lua_State *L)
{
	char ebuf[ERRMAX];

	if(loadimage(canvas, canvas->r, pixels->data, pixels->ndata) < 0){
		errstr(ebuf, sizeof ebuf);
		return luaL_error(L, "unable to update pixels: %s", ebuf);
	}
	return 0;
}

int
cconstrain(lua_State *L)
{
	double fa, fm, fM;
	int ia, im, iM;

	if(lua_isinteger(L, 1)){
		ia = luaL_checkinteger(L, 1);
		im = luaL_checkinteger(L, 2);
		iM = luaL_checkinteger(L, 3);
		if(ia < im)
			ia = im;
		else if(ia > iM)
			ia = iM;
		lua_pushinteger(L, ia);
	}else{
		fa = luaL_checknumber(L, 1);
		fm = luaL_checknumber(L, 2);
		fM = luaL_checknumber(L, 3);
		if(fa < fm)
			fa = fm;
		else if(fa > fM)
			fa = fM;
		lua_pushnumber(L, fa);
	}
	return 1;
}

int
cmap(lua_State *L)
{
	double v, s1, e1, s2, e2;
	int n;

	n = lua_gettop(L);
	if(n != 5)
		return luaL_error(L, "invalid argument count (expected 5 but got %d)", n);
	v = luaL_checknumber(L, 1);
	s1 = luaL_checknumber(L, 2);
	e1 = luaL_checknumber(L, 3);
	s2 = luaL_checknumber(L, 4);
	e2 = luaL_checknumber(L, 5);
	lua_pushnumber(L, map(v, s1, e1, s2, e2));
	return 1;
}

int
cnorm(lua_State *L)
{
	double v, s1, e1;
	int n;

	n = lua_gettop(L);
	if(n != 3)
		return luaL_error(L, "invalid argument count (expected 3 but got %d)", n);
	v = luaL_checknumber(L, 1);
	s1 = luaL_checknumber(L, 2);
	e1 = luaL_checknumber(L, 3);
	lua_pushnumber(L, map(v, s1, e1, 0.0, 1.0));
	return 1;
}

int
cdist(lua_State *L)
{
	int n, x1, y1, x2, y2;
	
	n = lua_gettop(L);
	if(n != 4)
		return luaL_error(L, "invalid argument count (expected 4 but got %d)", n);
	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	lua_pushnumber(L, dist(x1, y1, x2, y2));
	return 1;
}

int
cmag(lua_State *L)
{
	int n, x, y;

	n = lua_gettop(L);
	if(n != 2)
		return luaL_error(L, "invalid argument count (expected 2 but got %d)", n);
	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	lua_pushnumber(L, dist(0, 0, x, y));
	return 1;
}

int
clerp(lua_State *L)
{
	double r, a, b;
	int n;

	n = lua_gettop(L);
	if(n != 3)
		return luaL_error(L, "invalid argument count (expected 3 but got %d)", n);
	a = luaL_checknumber(L, 1);
	b = luaL_checknumber(L, 2);
	r = luaL_checknumber(L, 3);
	lua_pushnumber(L, lerp(a, b, r));
	return 1;
}

int
crandomgaussian(lua_State *L)
{
	double m, s, g;
	int n;

	m = 0.0;
	s = 1.0;
	n = lua_gettop(L);
	switch(n){
	case 2:
		s = luaL_checknumber(L, 2);
	case 1:
		m = luaL_checknumber(L, 1);
		break;
	case 0:
		break;
	default:
		luaL_error(L, "invalid argument count");
	}
	g = randomgaussian(m, s);
	lua_pushnumber(L, g);
	return 1;
}

void
registerfunc(lua_State *L, const char *name, int(*f)(lua_State*))
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
}

void
registermathfunc(lua_State *L, const char *name, lua_CFunction f)
{
	lua_getglobal(L, "math");
	lua_pushcfunction(L, f);
	lua_setfield(L, 1, name);
}

void
registerapi(lua_State *L)
{
	lset(L, "RGB", Crgb);
	lset(L, "HSV", Chsv);
	lset(L, "SQUARE", Endsquare);
	lset(L, "ROUND", Enddisc);

	registerfunc(L, "noLoop", cnoloop);
	registerfunc(L, "loop", cloop);
	registerfunc(L, "frameRate", cframerate);
	registerfunc(L, "size", csize);
	registerfunc(L, "colorMode", ccolormode);
	registerfunc(L, "background", cbackground);
	registerfunc(L, "noStroke", cnostroke);
	registerfunc(L, "strokeCap", cstrokecap);
	registerfunc(L, "stroke", cstroke);
	registerfunc(L, "strokeWeight", cstrokeweight);
	registerfunc(L, "noFill", cnofill);
	registerfunc(L, "fill", cfill);
	registerfunc(L, "point", cpoint);
	registerfunc(L, "line", cline);
	registerfunc(L, "square", csquare);
	registerfunc(L, "rect", crect);
	registerfunc(L, "circle", ccircle);
	registerfunc(L, "ellipse", cellipse);
	registerfunc(L, "arc", carc);
	registerfunc(L, "bezier", cbezier);
	registerfunc(L, "triangle", ctriangle);
	registerfunc(L, "quad", cquad);
	registerfunc(L, "translate", ctranslate);
	registerfunc(L, "rotate", crotate);
	registerfunc(L, "radians", cradians);
	registerfunc(L, "pushMatrix", cpushmatrix);
	registerfunc(L, "popMatrix", cpopmatrix);
	registerfunc(L, "pushStyle", cpushstyle);
	registerfunc(L, "popStyle", cpopstyle);
	registerfunc(L, "push", cpush);
	registerfunc(L, "pop", cpop);
	registerfunc(L, "color", ccolor);
	registerfunc(L, "lerpColor", clerpcolor);
	registerfunc(L, "loadPixels", cloadpixels);
	registerfunc(L, "updatePixels", cupdatepixels);
	registermathfunc(L, "dist", cdist);
	registermathfunc(L, "mag", cmag);
	registermathfunc(L, "constrain", cconstrain);
	registermathfunc(L, "map", cmap);
	registermathfunc(L, "norm", cnorm);
	registermathfunc(L, "lerp", clerp);
	registermathfunc(L, "randomGaussian", crandomgaussian);
}
