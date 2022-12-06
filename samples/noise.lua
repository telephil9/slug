#!/bin/slug

function setup()
	size(400, 400)
	background(0)
	loadPixels()
	for y = 1, 399 do
		for x = 1, 399 do
			n = math.noise(x * 0.01, y * 0.01)
			n = n + 1.0
			n = n / 2.0
			v = math.floor(255 * n)
			pixels[x + width * y] = color(v, v, v)
		end
	end
	updatePixels()
end

function draw()
	noLoop()
end
