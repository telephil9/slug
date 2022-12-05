#include "a.h"

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

