#include "a.h"

Mousectl *mc;
Keyboardctl *kc;
int drawing;

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

void
resize(lua_State *L, int w, int h)
{
	int fd, n;
	char buf[255];

	fd = open("/dev/wctl", OWRITE|OCEXEC);
	if(fd < 0)
		sysfatal("open: %r");
	n = snprint(buf, sizeof buf, "resize -dx %d -dy %d", w, h);
	if(write(fd, buf, n) != n)
		fprint(2, "write error: %r\n");
	close(fd);
	lua_pushnumber(L, w);
	lua_setglobal(L, "width");
	lua_pushnumber(L, h);
	lua_setglobal(L, "height");
}

void
threadmain(int argc, char *argv[])
{
	lua_State *L;
	Mouse m;
	Rune k;
	const char *s;
	int r;
	Alt alts[] = {
		{ nil, &m, CHANRCV },
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
	alts[0].c = mc->c;
	alts[1].c = mc->resizec;
	alts[2].c = kc->c;
	L = luaL_newstate();
	luaL_openlibs(L);
	r = luaL_dofile(L, argc > 1 ? argv[1] : NULL);
	if(r != LUA_OK){
		s = luaL_checkstring(L, lua_gettop(L));
		fprint(2, "error: %s\n", s);
	}
	registerfuncs(L);
	initstate();
	resize(L, width, height);
	drawing = 0;
	lsetup(L);
	drawing = 1;
	for(;;){
		ldraw(L);
		switch(alt(alts)){
		case 0:
			break;
		case 1:
			if(getwindow(display, Refnone)<0)
				sysfatal("getwindow: %r");
			resize(L, width, height);
			drawcanvas();
			break;
		case 2:
			if(k == Kdel)
				goto Done;
			break;
		}
	}
Done:
	lua_close(L);
	threadexitsall(r == LUA_OK ? nil : "error");
}

