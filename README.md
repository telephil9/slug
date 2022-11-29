slug
=====
slug (Simple LUA graphics) is a graphics programming tool inspired by [Processing](https://processing.org).  

Installation:
-------------
slug uses the [lu9-lua](https://git.sr.ht/~kvik/lu9-lua) native lua port to 9front by kvik.  
1) Clone the slug repository somewhere.  
2) Clone the lu9-lua repository as lua within the slug directory
3) mk && mk install

Usage:
------
A slug program is composed of two functions:
- setup (not mandatory) which is called before the main draw loop.
- draw which is called in a loop until program exits.

slug provides the following functions:
- background(color) paint the background using given color (see colors)
- stroke(color) change the shapes stroke color
- strokeWidth(n) change the width of strokes
- line(x1, y1, x2, y2) draw a line between points (x1,y1) and (x2, y2)

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

