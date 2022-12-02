#!/bin/slug

function setup()
	size(600, 600)
end

function draw()
	background(0)
	translate(width/2, height/2)
	stroke(255)
	strokeWidth(5)
	line(0, 0, mouseX, mouseY)
end

