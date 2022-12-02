#!/bin/slug

function setup()
	size(400, 400)
end

function draw()
	background(200)
	ellipse(0, 200, 132, 132)

	pushStyle()
	strokeWeight(40)
	fill(204, 153, 0)
	ellipse(200, 200, 132, 132)
	popStyle()

	ellipse(400, 200, 132, 132)
end

