#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void	lset(lua_State*, const char*, int);
void	resize(lua_State*, int, int);

void	drawcanvas(void);
void	initstate(lua_State*);
void	registerfuncs(lua_State*);

Image*	color(int, int, int);
Image*	grayscale(int);

extern int drawing;
extern Image* canvas;
extern int width;
extern int height;
extern Point origin;
