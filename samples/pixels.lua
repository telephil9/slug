#!/bin/slug

function setup()
	size(255, 255)
	background(0)
	loadPixels()
	for y=1,254 do
		c = color(y, y, y)
		for x=1, 254 do
			pixels[x + width * y] = c
		end
	end
	updatePixels()
end

function draw()
	noLoop()
end

