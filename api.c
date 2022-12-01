#include "a.h"

Image	*canvas;
int		width;
int		height;
int		nostroke;
int		strokecap;
Image	*stroke;
int		strokewidth;
int		nofill;
Image	*fill;
Point	origin;

void
initstate(lua_State *L)
{
	Rectangle r;

	width = 500;
	height = 500;
	r = Rect(0, 0, width, height);
	canvas = allocimage(display, r, screen->chan, 0, DWhite);
	nostroke = 0;
	strokecap = Endsquare;
	stroke = display->black;
	strokewidth = 1;
	nofill = 0;
	fill = display->white;
	origin = ZP;
	lset(L, "mouseX", 0);
	lset(L, "mouseY", 0);
}

void
drawcanvas(void)
{
	draw(screen, screen->r, canvas, nil, ZP);
	flushimage(display, 1);
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
		r = luaL_checkinteger(L, 1);
		g = r;
		b = r;
	}else if(c == 3){
		r = luaL_checkinteger(L, 1);
		g = luaL_checkinteger(L, 2);
		b = luaL_checkinteger(L, 3);
	}else{
		fprint(2, "invalid color request\n");
		return nil;
	}
	i = color(r, g, b);
	return i;
}

int
cbackground(lua_State *L)
{
	Image *i;

	i = getcolor(L);
	if(i == nil)
		return 1;
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
	if(i == nil)
		return 1;
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
	if(i == nil)
		return 1;
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
	p1 = addpt(origin, Pt(x, y));
	p2 = addpt(p1, Pt(strokewidth, strokewidth));
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
	p1 = addpt(origin, Pt(x1, y1));
	p2 = addpt(origin, Pt(x2, y2));
	if(!nostroke)
		line(canvas, p1, p2, strokecap, strokecap, strokewidth, stroke, ZP);
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
	p1 = addpt(origin, Pt(x, y));
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
	p1 = addpt(origin, Pt(x, y));
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
	p = addpt(origin, Pt(x, y));
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
	p = addpt(origin, Pt(x, y));
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
	p = addpt(origin, Pt(x, y));
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
	p[0] = addpt(origin, Pt(x1, y1));
	p[1] = addpt(origin, Pt(x2, y2));
	p[2] = addpt(origin, Pt(x3, y3));
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
	p[0] = addpt(origin, Pt(x1, y1));
	p[1] = addpt(origin, Pt(x2, y2));
	p[2] = addpt(origin, Pt(x3, y3));
	p[3] = addpt(origin, Pt(x4, y4));
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

void
registerfunc(lua_State *L, const char *name, int(*f)(lua_State*))
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
}

void
registerapi(lua_State *L)
{
	lset(L, "SQUARE", Endsquare);
	lset(L, "ROUND", Enddisc);

	registerfunc(L, "size", csize);
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
}

