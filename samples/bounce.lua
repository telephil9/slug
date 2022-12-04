#!/bin/slug

function setup()
	size(400, 400)
end

x = math.random(400)
y = math.random(400)
vx = 1
vy = 1
function draw()
	background(200)
	fill(255, 100, 100)
	circle(x, y, 20)
	x = x + vx
	if x < 10 then
		x = 10
		vx = -vx
	elseif x > width - 10 then
		x = width - 10
		vx = -vx
	end
	y = y + vy
	if y < 10 then
		y = 10
		vy = -vy
	elseif y > height - 10 then
		y = height - 10
		vy = -vy
	end
end

