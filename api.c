#include "a.h"

Image *canvas;
int nostroke;
Image *stroke;
int strokewidth;
int nofill;
Image *fill;

void
initstate(void)
{
	Rectangle r;

	r = rectsubpt(screen->r, screen->r.min);
	canvas = allocimage(display, r, screen->chan, 0, DNofill);
	nostroke = 0;
	stroke = display->black;
	strokewidth = 1;
	nofill = 0;
	fill = display->white;
}

void
drawcanvas(void)
{
	draw(screen, screen->r, canvas, nil, ZP);
	flushimage(display, 1);
}

int
cbackground(lua_State *L)
{
	Image *i;
	int n;

	n = luaL_checkinteger(L, 1);
	i = getcolor(n);
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
cstroke(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	stroke = getcolor(n);
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
	int n;

	n = luaL_checkinteger(L, 1);
	fill = getcolor(n);
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
cline(lua_State *L)
{
	Point p1, p2;
	int x1, x2, y1, y2;

	x1 = luaL_checkinteger(L, 1);
	y1 = luaL_checkinteger(L, 2);
	x2 = luaL_checkinteger(L, 3);
	y2 = luaL_checkinteger(L, 4);
	p1 = Pt(x1, y1);
	p2 = Pt(x2, y2);
	if(!nostroke)
		line(canvas, p1, p2, 0, 0, strokewidth, stroke, ZP);
	return 0;
}

int
csquare(lua_State *L)
{
	Rectangle r;
	Point p1, p2;
	int x, y, w;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	w = luaL_checkinteger(L, 3);
	p1 = Pt(x, y);
	p2 = addpt(p1, Pt(w, w));
	r = Rpt(p1, p2);
	if(!nofill)
		draw(canvas, r, fill, nil, ZP);
	if(!nostroke)
		border(canvas, r, strokewidth, stroke, ZP);
	return 0;
}

int
crect(lua_State *L)
{
	Rectangle r;
	Point p1, p2;
	int x, y, w, h;

	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);
	w = luaL_checkinteger(L, 3);
	h = luaL_checkinteger(L, 4);
	p1 = Pt(x, y);
	p2 = addpt(p1, Pt(w, h));
	r = Rpt(p1, p2);
	if(!nofill)
		draw(canvas, r, fill, nil, ZP);
	if(!nostroke)
		border(canvas, r, strokewidth, stroke, ZP);
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
	p = Pt(x, y);
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
	p = Pt(x, y);
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
	p = Pt(x, y);
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
	p[0] = Pt(x1, y1);
	p[1] = Pt(x2, y2);
	p[2] = Pt(x3, y3);
	p[3] = p[0];
	if(!nofill)
		fillpoly(canvas, p, 3, 0, fill, ZP);
	if(!nostroke)
		poly(canvas, p, 4, 0, 0, strokewidth, stroke, ZP);
	return 0;
}

void
registerfunc(lua_State *L, const char *name, int(*f)(lua_State*))
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
}

void
registerfuncs(lua_State *L)
{
	registerfunc(L, "background", cbackground);
	registerfunc(L, "noStroke", cnostroke);
	registerfunc(L, "stroke", cstroke);
	registerfunc(L, "strokeWidth", cstrokewidth);
	registerfunc(L, "noFill", cnofill);
	registerfunc(L, "fill", cfill);
	registerfunc(L, "line", cline);
	registerfunc(L, "square", csquare);
	registerfunc(L, "rect", crect);
	registerfunc(L, "circle", ccircle);
	registerfunc(L, "ellipse", cellipse);
	registerfunc(L, "arc", carc);
	registerfunc(L, "triangle", ctriangle);
}
