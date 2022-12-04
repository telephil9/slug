slug
=====
slug (Simple LUA graphics) is a graphics programming tool inspired by [Processing](https://processing.org).  

Installation:
-------------
slug uses the [lu9-lua](https://git.sr.ht/~kvik/lu9-lua) native lua port to 9front by kvik.  
``` sh
% git/clone <repository_url>
% cd slug
% mk pull
% mk
% mk install
```

Usage:
------
A basic `slug` script:
```lua
#!/bin/slug

function setup()
	size(300, 300)
end

function draw()
	background(200)
	stroke(255, 0, 255)
	strokeWidth(5)
	line(10, 10, 100, 100)
end
```
See `slug(1)` for the complete documentation.

License:
--------
MIT

Bugs:
-----
This is a really quick proof of concept so bugs most certainly exist.

