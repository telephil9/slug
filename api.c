#include "a.h"

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
int		strokewidth;
int		nofill;
Image	*fill;
Point	origin;
double	angle;

void
initstate(lua_State *L)
{
	Rectangle r;

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
	strokewidth = 1;
	nofill = 0;
	fill = display->white;
	origin = ZP;
	angle = 0.0;
	lset(L, "mouseX", 0);
	lset(L, "mouseY", 0);
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
	resize(L, w, h);
	return 0;
}

Image*
getcolor(lua_State *L)
{
	Image *i;
	int c, r, g, b;

	c = lua_gettop(L);
	if(c == 1){
		g = luaL_checkinteger(L, 1);
		i = color(g, g, g, 0);
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
cstrokewidth(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	strokewidth = n;
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
	p2 = canvaspt(x + strokewidth, y + strokewidth);
	if(!nostroke)
		if(strokecap == Endsquare)
			draw(canvas, Rpt(p1, p2), stroke, nil, ZP);
		else if(strokecap == Enddisc)
			fillellipse(canvas, p1, strokewidth/2.0, strokewidth/2.0, stroke, ZP);
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
		line(canvas, p1, p2, strokecap, strokecap, strokewidth, stroke, ZP);
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
		poly(canvas, p, 5, strokecap, strokecap, strokewidth, stroke, ZP);
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
		poly(canvas, p, 5, strokecap, strokecap, strokewidth, stroke, ZP);
	return 0;
}

int
ccircle(lua_State *L)
{
	Point p;
	int x, y, a;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	a = luaL_checkinteger(L, 3);
	p = canvaspt(x, y);
	if(!nofill)
		fillellipse(canvas, p, a, a, fill, ZP);
	if(!nostroke)
		ellipse(canvas, p, a, a, strokewidth, stroke, ZP);
	return 0;
}

int
cellipse(lua_State *L)
{
	Point p;
	int x, y, a, b;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	a = luaL_checkinteger(L, 3);
	b = luaL_checkinteger(L, 4);
	p = canvaspt(x, y);
	if(!nofill)
		fillellipse(canvas, p, a, b, fill, ZP);
	if(!nostroke)
		ellipse(canvas, p, a, b, strokewidth, stroke, ZP);
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
	c = luaL_checkinteger(L, 5);
	d = luaL_checkinteger(L, 6);
	p = canvaspt(x, y);
	if(!nofill)
		fillarc(canvas, p, a, b, fill, ZP, c, d);
	if(!nostroke)
		arc(canvas, p, a, b, strokewidth, stroke, ZP, c, d);
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
		poly(canvas, p, 4, strokecap, strokecap, strokewidth, stroke, ZP);
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
		poly(canvas, p, 5, strokecap, strokecap, strokewidth, stroke, ZP);
	return 0;

}

int
ctranspose(lua_State *L)
{
	int x, y;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	origin = Pt(x, y);
	return 0;
}

int
crotate(lua_State *L)
{
	angle = luaL_checknumber(L, 1);
	return 0;
}

void
registerfunc(lua_State *L, const char *name, int(*f)(lua_State*))
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
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
	registerfunc(L, "strokeWidth", cstrokewidth);
	registerfunc(L, "noFill", cnofill);
	registerfunc(L, "fill", cfill);
	registerfunc(L, "point", cpoint);
	registerfunc(L, "line", cline);
	registerfunc(L, "square", csquare);
	registerfunc(L, "rect", crect);
	registerfunc(L, "circle", ccircle);
	registerfunc(L, "ellipse", cellipse);
	registerfunc(L, "arc", carc);
	registerfunc(L, "triangle", ctriangle);
	registerfunc(L, "quad", cquad);
	registerfunc(L, "transpose", ctranspose);
	registerfunc(L, "rotate", crotate);
}

