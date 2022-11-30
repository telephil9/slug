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

void	resize(int, int);

void	drawcanvas(void);
void	initstate(void);
void	registerfuncs(lua_State*);

Image*	getcolor(int);

extern int drawing;
extern Image* canvas;
extern int width;
extern int height;
