#include "a.h"

Mousectl *mc;
Keyboardctl *kc;
int drawing;

lua_State*
linit(int argc, char *argv[])
{
	lua_State *L;
	const char *s;
	int r;

	L = luaL_newstate();
	luaL_openlibs(L);
	r = luaL_dofile(L, argc > 1 ? argv[1] : NULL);
	if(r != LUA_OK){
		s = luaL_checkstring(L, lua_gettop(L));
		fprint(2, "error: %s\n", s);
	}
	return L;
}

int
lcallerror(lua_State *L)
{
	const char *m;

	m = lua_tostring(L, 1);
	if(m == nil){
		if(luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING)
			return 1;
		else
			m = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
	}
	luaL_traceback(L, L, m, 1);
	return 1;
}	

void
lcall(lua_State *L, char *fn)
{
	int r, base;

	lua_getglobal(L, fn);
	if(!lua_isfunction(L, -1))
		return;
	base = lua_gettop(L);
	lua_pushcfunction(L, lcallerror);
	lua_insert(L, base);
	r = lua_pcall(L, 0, 0, base);
	lua_remove(L, base);
	if(r != LUA_OK){
		fprint(2, "error: %s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}
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
	L = linit(argc, argv);
	registerfuncs(L);
	initstate();
	resize(L, width, height);
	drawing = 0;
	lcall(L, "setup");
	drawing = 1;
	for(;;){
		lcall(L, "draw");
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
	threadexitsall(nil);
}

