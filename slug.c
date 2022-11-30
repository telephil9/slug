#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct Color Color;

struct Color
{
	ulong color;
	Image *i;
};

#include "colors.h"

lua_State *L;
Mousectl *mc;
Keyboardctl *kc;

int nostroke;
Image *stroke;
int strokewidth;
int nofill;
Image *fill;

void
lsetup(void)
{
	lua_getglobal(L, "setup");
	if(!lua_isfunction(L, -1))
		return;
	lua_call(L, 0, 0);
	flushimage(display, 1);
}

void
ldraw(void)
{
	lua_getglobal(L, "draw");
	if(!lua_isfunction(L, -1))
		return;
	lua_call(L, 0, 0);
	flushimage(display, 1);
}

Image*
getcolor(int n)
{
	if(n < 0)
		n = 0;
	if(n >= nelem(palette))
		n = nelem(palette) - 1;
	if(palette[n].i == nil)
		palette[n].i = allocimage(display, Rect(0, 0, 1, 1), screen->chan, 1, palette[n].color);
	return palette[n].i;
}

int
cbackground(lua_State *L)
{
	Image *i;
	int n;

	n = luaL_checkinteger(L, 1);
	i = getcolor(n);
	draw(screen, screen->r, i, nil, ZP);
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
	p1 = addpt(screen->r.min, Pt(x1, y1));
	p2 = addpt(screen->r.min, Pt(x2, y2));
	if(!nostroke)
		line(screen, p1, p2, 0, 0, strokewidth, stroke, ZP);
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
	p1 = addpt(screen->r.min, Pt(x, y));
	p2 = addpt(p1, Pt(w, w));
	r = Rpt(p1, p2);
	if(!nofill)
		draw(screen, r, fill, nil, ZP);
	if(!nostroke)
		border(screen, r, strokewidth, stroke, ZP);
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
	p1 = addpt(screen->r.min, Pt(x, y));
	p2 = addpt(p1, Pt(w, h));
	r = Rpt(p1, p2);
	if(!nofill)
		draw(screen, r, fill, nil, ZP);
	if(!nostroke)
		border(screen, r, strokewidth, stroke, ZP);
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
	p = addpt(screen->r.min, Pt(x, y));
	if(!nofill)
		fillellipse(screen, p, a, a, fill, ZP);
	if(!nostroke)
		ellipse(screen, p, a, a, strokewidth, stroke, ZP);
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
	p = addpt(screen->r.min, Pt(x, y));
	if(!nofill)
		fillellipse(screen, p, a, b, fill, ZP);
	if(!nostroke)
		ellipse(screen, p, a, b, strokewidth, stroke, ZP);
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
	p = addpt(screen->r.min, Pt(x, y));
	if(!nofill)
		fillarc(screen, p, a, b, fill, ZP, c, d);
	if(!nostroke)
		arc(screen, p, a, b, strokewidth, stroke, ZP, c, d);
	return 0;
}

void
initcontext(void)
{
	nostroke = 0;
	stroke = display->black;
	strokewidth = 1;
	nofill = 0;
	fill = display->white;
}

void
registerfunc(const char *name, int(*f)(lua_State*))
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
}

void
registerfuncs(void)
{
	registerfunc("background", cbackground);
	registerfunc("noStroke", cnostroke);
	registerfunc("stroke", cstroke);
	registerfunc("strokeWidth", cstrokewidth);
	registerfunc("noFill", cnofill);
	registerfunc("fill", cfill);
	registerfunc("line", cline);
	registerfunc("square", csquare);
	registerfunc("rect", crect);
	registerfunc("circle", ccircle);
	registerfunc("ellipse", cellipse);
	registerfunc("arc", carc);
}

void
threadmain(int argc, char *argv[])
{
	Rune k;
	const char *s;
	int r;
	Alt alts[] = {
		{ nil, nil, CHANRCV },
		{ nil, &k,  CHANRCV },
		{ nil, nil, CHANNOBLK },
	};

	if(initdraw(nil, nil, "slug") < 0)
		sysfatal("initdraw: %r");
	display->locking = 0;
	if((mc = initmouse(nil, screen)) == nil)
		sysfatal("initmouse: %r");
	if((kc = initkeyboard(nil)) == nil)
		sysfatal("initkeyboard: %r");
	alts[0].c = mc->resizec;
	alts[1].c = kc->c;
	L = luaL_newstate();
	luaL_openlibs(L);
	r = luaL_dofile(L, argc > 1 ? argv[1] : NULL);
	if(r != LUA_OK){
		s = luaL_checkstring(L, lua_gettop(L));
		fprint(2, "error: %s\n", s);
	}
	registerfuncs();
	initcontext();
	lsetup();
	for(;;){
		ldraw();
		switch(alt(alts)){
		case 0:
			if(getwindow(display, Refnone)<0)
				sysfatal("getwindow: %r");
			break;
		case 1:
			if(k == Kdel)
				goto Done;
			break;
		}
	}
Done:
	lua_close(L);
	threadexitsall(r == LUA_OK ? nil : "error");
}

