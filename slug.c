#include "a.h"
#include "colors.h"

Mousectl *mc;
Keyboardctl *kc;

void
lsetup(lua_State *L)
{
	lua_getglobal(L, "setup");
	if(!lua_isfunction(L, -1))
		return;
	lua_call(L, 0, 0);
	drawcanvas();
}

void
ldraw(lua_State *L)
{
	lua_getglobal(L, "draw");
	if(!lua_isfunction(L, -1))
		return;
	lua_call(L, 0, 0);
	drawcanvas();
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

void
threadmain(int argc, char *argv[])
{
	lua_State *L;
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
	registerfuncs(L);
	initstate();
	lsetup(L);
	for(;;){
		ldraw(L);
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

