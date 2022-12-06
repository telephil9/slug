#include "a.h"

double
lerp(double r, double a, double b)
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

/* Perlin noise function reference implementation by Ken Perlin */

int p[512] = { 151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 
               103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 
               26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 
               87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 
               77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 
               46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 
               187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 
               198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 
               255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 
               170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 
               172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 
               104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 
               241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 
               157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 
               93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

double
fade(double t) 
{ 
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double
grad(int hash, double x, double y, double z) 
{
	int h;
	double u, v;

	h = hash & 15;
	u = h<8 ? x : y;
	v = h<4 ? y : h==12||h==14 ? x : z;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}
   
double
noise(double x, double y, double z)
{
	int X, Y, Z, A, B, AA, AB, BA, BB;
	double u, v, w;

	X = (int)floor(x) & 255;
	Y = (int)floor(y) & 255;
	Z = (int)floor(z) & 255;
	x -= floor(x);                                
	y -= floor(y);                                
	z -= floor(z);
	u = fade(x);
	v = fade(y);
	w = fade(z);
	A = p[X  ]+Y;
	AA = p[A]+Z;
	AB = p[A+1]+Z;
	B = p[X+1]+Y;
	BA = p[B]+Z;
	BB = p[B+1]+Z;

 	return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                   grad(p[BA  ], x-1, y  , z   )),
                           lerp(u, grad(p[AB  ], x  , y-1, z   ),
                                   grad(p[BB  ], x-1, y-1, z   ))),
                   lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ), 
                                   grad(p[BA+1], x-1, y  , z-1 )), 
                           lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                   grad(p[BB+1], x-1, y-1, z-1 ))));
}
