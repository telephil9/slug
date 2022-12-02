#!/bin/slug

function setup()
	size(400, 400)
end

function draw()
	background(150)

	fill(255)
	rect(0, 0, 200, 200)

	pushMatrix()
	transpose(120, 80)
	fill(0)  
	rect(0, 0, 200, 200)
	popMatrix()

	fill(100)  
	rect(60, 40, 200, 200)
end

