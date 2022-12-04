#include "a.h"

Pixels	*pixels;

Pixels*
checkpixels(lua_State *L)
{
	void *p;

	p = luaL_checkudata(L, 1, "slug.pixels");
	luaL_argcheck(L, p != nil, 1, "`pixels` expected");
	return p;
}

int
pixelstostring(lua_State *L)
{
	checkpixels(L);
	lua_pushfstring(L, "pixels(%dx%d)", width, height);
	return 1;
}

int
pixelsget(lua_State *L)
{
	Pixels *p;
	int i;

	p = checkpixels(L);
	i = luaL_checkinteger(L, 2);
	luaL_argcheck(L, 1 <= i && i <= (width*height), 2, "index out of range");
	i = (i-1)*4;
	lua_newtable(L);
	lua_pushinteger(L, p->data[i+2]);
	lua_setfield(L, -2, "r");
	lua_pushinteger(L, p->data[i+1]);
	lua_setfield(L, -2, "g");
	lua_pushinteger(L, p->data[i+0]);
	lua_setfield(L, -2, "b");
	return 1;
}

int
pixelsset(lua_State *L)
{
	Pixels *p;
	int i, r, g, b;

	p = checkpixels(L);
	i = luaL_checkinteger(L, 2);
	luaL_argcheck(L, 1 <= i && i <= width*height, 2, "index out of range");
	if(lua_istable(L, 3) == 0)
		luaL_argerror(L, 3, "color table expected");
	lua_pushstring(L, "r");
	lua_gettable(L, 3);
	r = luaL_checkinteger(L, -1);
	lua_pushstring(L, "g");
	lua_gettable(L, 3);
	g = luaL_checkinteger(L, -1);
	lua_pushstring(L, "b");
	lua_gettable(L, 3);
	b = luaL_checkinteger(L, -1);
	i = (i-1)*4;
	p->data[i + 2] = r;
	p->data[i + 1] = g;
	p->data[i + 0] = b;
	return 0;
}

static const struct luaL_Reg pixelsmethods [] = {
	{ "__tostring", pixelstostring },
	{ "__index", pixelsget },
	{ "__newindex", pixelsset },
	{ nil, nil},
};

int
registerpixels(lua_State *L)
{
	luaL_newmetatable(L, "slug.pixels");
	luaL_setfuncs(L, pixelsmethods, 0);
	lua_pop(L, 1);
	return 0;
}

void
lpushpixels(lua_State *L)
{
	int n;

	n = sizeof(Pixels) + (width*height*4-1)*sizeof(uchar);
	pixels = lua_newuserdata(L, n);
	luaL_setmetatable(L, "slug.pixels");
	pixels->ndata = width*height*4;
	lua_setglobal(L, "pixels");
}

