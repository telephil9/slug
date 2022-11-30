slug
=====
slug (Simple LUA graphics) is a graphics programming tool inspired by [Processing](https://processing.org).  

Installation:
-------------
slug uses the [lu9-lua](https://git.sr.ht/~kvik/lu9-lua) native lua port to 9front by kvik.  
1) Clone the slug repository somewhere.  
2) Clone the lu9-lua repository as lua within the slug directory (`cd slug; git/clone https://git.sr.ht/~kvik/lu9-lua lua`)
3) `mk && mk install`

Usage:
------
A slug program is written in lua and composed of two functions:
- `setup()` (not mandatory) which is called before the main draw loop.
- `draw()` which is called in a loop until program exits.

slug provides the following functions:
- `background(color)` paint the background using given color (see colors)
- `noStroke()` disable drawing shapes outline
- `stroke(color)` change the shapes outline color
- `strokeWidth(n)` change the width of outlines
- `noFill()` disable filling shapes
- `fill(color)` change the shapes fill color
- `line(x1, y1, x2, y2)` draw a line between points (x1,y1) and (x2, y2)
- `square(x, y, w)` draw a square with a top left corner at point (x,y) and size w
- `rect(x, y, w, h)` draw a rectangle with a top left corner at point (x,y), width w and height h
- `circle(x, y, r)` draw a circle centered on point (x,y) and a radius r
- `ellipse(x, y, a, b)` draw an ellipse centered on point (x,y) with an horizontal radius of a and a vertical radius of b
- `arc(x, y, a, b, c, d)` draw an arc centered on point (x,y) with an horizontal radius of a and a vertical radius of b. arc is like ellipse, but draws only that portion of the ellipse starting at angle c and extending through an angle of d

colors:  
slug currently uses a 216 color palette based on web safe color palette.  
color parameter is thus a number in the range [0;216[

Sample:
-------
```lua
function draw()
	background(0)
	stroke(255)
	strokeWidth(5)
	line(10, 10, 100, 100)
end
```

License:
--------
MIT

Bugs:
-----
This is a really quick proof of concept so bugs most certainly exist.

