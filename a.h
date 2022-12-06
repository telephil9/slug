#include <u.h>
#include <libc.h>
#include <thread.h>
#include <draw.h>
#include <mouse.h>
#include <keyboard.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct Pixels Pixels;

struct Pixels
{
	int ndata;
	uchar data[1];
};

void	lset(lua_State*, const char*, int);
void	resize(lua_State*, int, int);

void	drawcanvas(void);
void	initstate(lua_State*);
void	reset(void);
void	registerapi(lua_State*);

void	hsvtorgb(int, int, int, int*, int*, int*);
Image*	color(int, int, int, int);

int		registerpixels(lua_State*);
void	lpushpixels(lua_State*);

double	dist(int, int, int, int);
double	map(double, double, double, double, double);
double	lerp(double, double, double);
double	randomgaussian(double, double);
double	noise(double, double, double);

extern int		drawing;
extern int		looping;
extern int		framerate;
extern Image	*canvas;
extern Pixels	*pixels;
extern int		width;
extern int		height;
extern Point	origin;

