#!/bin/slug

function setup()
	size(400, 400)
	background(200)
	for y = 0, 399 do
		x = math.floor(math.randomGaussian() * 60)
		line(200, y, 200 + x, y)
	end
end

function draw()
end

