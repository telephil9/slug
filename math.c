#include "a.h"

double
lerp(double a, double b, double r)
{
	if(r < 0.0)
		r = 0.0;
	else if(r > 1.0)
		r = 1.0;
	return r * (b - a) + a;
}

double
dist(int x1, int y1, int x2, int y2)
{
	int dx = x1 - x2;
	int dy = y1 - y2;
	return sqrt(dx*dx + dy*dy);
}

double
map(double v, double s1, double e1, double s2, double e2)
{
	double r;

	r = (v - s1) / (e1 - s1) * (e2 - s2) + s2;
	return r;
}

double
randomgaussian(double mean, double sd)
{
	static int prev = 0;
	static double y2;
	double y1, x1, x2, w;

	if(prev){
		y1 = y2;
		prev = 0;
	}else{
		do{
			x1 = 2*frand() - 1;
			x2 = 2*frand() - 1;
			w = x1 * x1 + x2 * x2;
		}while(w >= 1);
		w = sqrt(-2 * log(w) / w);
		y1 = x1 * w;
		y2 = x2 * w;
		prev = 1;
	}
	return y1 * sd + mean;
}

