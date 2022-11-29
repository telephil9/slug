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
Image *fg;
int strokewidth;

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
cstroke(lua_State *L)
{
	int n;

	n = luaL_checkinteger(L, 1);
	fg = getcolor(n);
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
	line(screen, p1, p2, 0, 0, strokewidth, fg, ZP);
	return 0;
}

void
initcontext(void)
{
	fg = display->black;
	strokewidth = 1;
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
	registerfunc("stroke", cstroke);
	registerfunc("strokeWidth", cstrokewidth);
	registerfunc("line", cline);
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

