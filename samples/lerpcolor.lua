#!/bin/slug

function setup()
	size(400,400)
	background(51)
	stroke(255)
	from = color(204, 102, 0)
	to = color(0, 102, 153)
	interA = lerpColor(from, to, .33)
	interB = lerpColor(from, to, .66)
	fill(from)
	rect(40, 80, 80, 240)
	fill(interA)
	rect(120, 80, 80, 240)
	fill(interB)
	rect(200, 80, 80, 240)
	fill(to)
	rect(280, 80, 80, 240)
end

function draw()
end

