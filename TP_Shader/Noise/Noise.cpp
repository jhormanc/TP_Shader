#include "Noise.h"

#include <stdlib.h>
#include <math.h>

static float unit = 1.0f / sqrt(2.f);

static float grad2[][2] = { { unit, unit }, { -unit, unit }, { unit, -unit }, { -unit, -unit },
{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };

static int grad3[][3] = { { 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 } };

static int p[] = { 151, 160, 137, 91, 90, 15,
131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

// To remove the need for index wrapping, float the permutation table length
static int perm[512];

// This method is a *lot* faster than using (int)Math.floor(x)
int Noise::fastfloor(float x)
{
	return x>0 ? (int)x : (int)x - 1;
}

float Noise::dot(int g[], float x, float y)
{
	return g[0] * x + g[1] * y;
}

// Renvoi une valeur aleatoire entre 0 et 1 a partir de x.
float Noise::hash(float x)
{
	return (1.0 + (sin(sin(x) * rand()))) / 2.0;
}

float Noise::ridge(const float z, const float zr)
{
	if (z < zr)
		return z;
	return 2 * zr - z;
}

Vector Noise::warp(const Vector p, const float c, const float f, const bool perlin)
{
	if (perlin)
		return p + Vector((f * p.x, f * p.y), perlin2D(f * (p.x * cos(30.f) - p.y * sin(30.f)) + 10., f * (p.x * sin(30.f) + p.y * cos(30.f)) + 10.), 0.) * c;

	return p + Vector((f * p.x, f * p.y), simplex(f * (p.x * cos(30.f) - p.y * sin(30.f)) + 10., f * (p.x * sin(30.f) + p.y * cos(30.f)) + 10.), 0.) * c;
}

float Noise::smooth(const float z, const float zmin, const float zmax)
{
	if (z < zmin)
		return 0;

	if (z > zmax)
		return 1;

	float t = (z - zmin) / (zmax - zmin);

	return 1 - (1 - t * t) * (1 - t * t);
}

// 2D Simplex noise
float Noise::simplex(const float xin, const float yin)
{
	for (int i = 0; i < 512; i++)
		perm[i] = p[i & 255];

	float n0, n1, n2; // Noise contributions from the three corners
	// Skew the input space to determine which Noise cell we're in
	float F2 = 0.5 * (sqrt(3.0) - 1.0);
	float s = (xin + yin) * F2; // Hairy factor for 2D
	int i = fastfloor(xin + s);
	int j = fastfloor(yin + s);
	float G2 = (3.0 - sqrt(3.0)) / 6.0;
	float t = (i + j) * G2;
	float X0 = i - t; // Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = xin - X0; // The x,y distances from the cell origin
	float y0 = yin - Y0;

	// For the 2D case, the Noise shape is an equilateral triangle.
	// Determine which Noise we are in.
	int i1, j1; // Offsets for second (middle) corner of Noise in (i,j) coords
	if (x0  > y0)
	{
		i1 = 1;
		j1 = 0;
	} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else
	{
		i1 = 0;
		j1 = 1;
	} // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0 + 2.0 * G2;

	// Work out the hashed gradient indices of the three Noise corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	// Calculate the contribution from the three corners
	float t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 < 0)
		n0 = 0.0;
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}

	float t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 < 0)
		n1 = 0.0;
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}

	float t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 < 0)
		n2 = 0.0;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * (n0 + n1 + n2);
}

// Perlin noise
// Renvoie un float compris entre -1 et 1
float Noise::perlin2D(const float x, const float y)
{
	float tempX, tempY;
	int x0, y0, ii, jj, gi0, gi1, gi2, gi3;

	float tmp, s, t, u, v, Cx, Cy, Li1, Li2;

	// On récupère les positions de la grille associée à (x,y)
	x0 = (int)(x);
	y0 = (int)(y);

	// Masquage
	ii = x0 & 255;
	jj = y0 & 255;

	// Pour récupérer les vecteurs
	gi0 = p[ii + p[jj]] % 8;
	gi1 = p[ii + 1 + p[jj]] % 8;
	gi2 = p[ii + p[jj + 1]] % 8;
	gi3 = p[ii + 1 + p[jj + 1]] % 8;

	// On récupère les vecteurs et on pondère
	tempX = x - x0;
	tempY = y - y0;
	s = grad2[gi0][0] * tempX + grad2[gi0][1] * tempY;

	tempX = x - (x0 + 1);
	tempY = y - y0;
	t = grad2[gi1][0] * tempX + grad2[gi1][1] * tempY;

	tempX = x - x0;
	tempY = y - (y0 + 1);
	u = grad2[gi2][0] * tempX + grad2[gi2][1] * tempY;

	tempX = x - (x0 + 1);
	tempY = y - (y0 + 1);
	v = grad2[gi3][0] * tempX + grad2[gi3][1] * tempY;


	// Lissage
	tmp = x - x0;
	Cx = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

	Li1 = s + Cx*(t - s);
	Li2 = u + Cx*(v - u);

	tmp = y - y0;
	Cy = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

	return Li1 + Cy*(Li2 - Li1);
}

// Choix du noise
float Noise::noise(const float x, const float y)
{
	float plains = noisePlains(x, y);
	float mountains = noiseMountains(x, y);

	return plains + mountains * smooth(plains, 0.f, 5.f);
}

float Noise::noisePlains(const float x, const float y)
{
	Vector w = warp(Vector(x, y, 0.), 2., 1. / 50.0, false);
	float tmp;

	tmp = (simplex(w.x / 2000.f, w.y / 2000.f) + 1.) * 0.5f;
	float z0 = 3. * tmp;

	tmp = (simplex(x / 4000.f, y / 4000.f) + 1.) * 0.5f;
	float z1 = 5. * tmp;

	return z0 + z1 * smooth(z1, 0., 5.);
	return (z1 + z0) * smooth(z0, 0., 5.) * smooth(z1, 0., 5.);
}

float Noise::noiseMountains(const float x, const float y)
{
	Vector w = warp(Vector(x, y, 0.), 5., 1. / 50.0, false);
	float tmp;

	tmp = (simplex(w.x / 750., w.y / 750.) + 1.) * 0.5;
	float z0 = ridge(300. * tmp, 250.);

	tmp = (simplex(w.x / 400., w.y / 400.) + 1.) * 0.5;
	float z1 = 20. + 30. * tmp;

	tmp = (simplex(w.x / 250., w.y / 250.) + 1.) * 0.5;
	float z2 = ridge(30. * tmp, 40.);

	tmp = (simplex(w.x / 150., w.y / 150.) + 1.) * 0.5;
	float z3 = 15. * tmp;

	tmp = (simplex(w.x / 50., w.y / 50.) + 1.) * 0.5;
	float z4 = 7. * tmp;

	float z = z0 + z1 + z2 * smooth(z0, 50., 200.) + z3 * smooth(z0, 150., 200.) + z4 * smooth(z0, 150., 250.);
	z += (z1 * (1. - smooth(z, 0., 60.)));
	return z;
}

float Noise::noise1(const float x, const float y)
{
	Vector w = warp(Vector(x, y, 0.), 5., 1. / 50.0, false);
	float tmp;

	tmp = (simplex(w.x / 500., w.y / 500.) + 1.) * 0.5;
	float z0 = ridge(200. * tmp, 180.);

	tmp = (simplex(w.x / 200., w.y / 200.) + 1.) * 0.5;
	float z1 = 50. * tmp;

	tmp = (simplex(w.x / 50, w.y / 50) + 1.) * 0.5;
	float z2 = 15. * tmp;

	float z = z0 * smooth(z0, 50., 200.) + z1 * smooth(z0, 50., 200.) + z2 * smooth(z0, 50., 200.);
	return z;
}

float Noise::noise2(const float x, const float y)
{
	Vector w = warp(Vector(x, y, 0.), 5., 1. / 50.0, false);
	float tmp;

	tmp = (simplex(w.x / 700., w.y / 700.) + 1.) * 0.5;
	float z0 = 200. * tmp;

	tmp = (simplex(w.x / 300., w.y / 300.) + 1.) * 0.5;
	float z1 = 100. * tmp;

	tmp = (simplex(w.x / 100., w.y / 100.) + 1.) * 0.5;
	float z2 = 50. * tmp;

	tmp = (simplex(w.x / 30., w.y / 30.) + 1.) * 0.5;
	float z3 = 30. * tmp;

	float z = z0 - ridge(z0 * smooth(z0, 0., 100.), 100.);
	z += z1 * smooth(z, 50., 300.);
	z += z2 * smooth(z, 70., 300.);
	z += z3 * smooth(z, 100., 300.);

	return z;
}

float Noise::noise3(const float x, const float y)
{
	Vector w = warp(Vector(x, y, 0.), 5., 1. / 50.0, false);
	float tmp;

	tmp = (simplex(w.x / 500., w.y / 500.) + 1.) * 0.5;
	float z0 = ridge(250. * tmp, 230.);

	tmp = (simplex(w.x / 300., w.y / 300.) + 1.) * 0.5;
	float z1 = 20. + 30. * tmp;

	tmp = (simplex(w.x / 200., w.y / 200.) + 1.) * 0.5;
	float z2 = ridge(60. * tmp, 40.);

	tmp = (simplex(w.x / 50., w.y / 50.) + 1.) * 0.5;
	float z3 = 30. * tmp;

	tmp = (simplex(w.x / 5., w.y / 5.) + 1.) * 0.5;
	float z4 = 15. * tmp;

	float z = z0 + z1 + z2 * smooth(z0, 50., 200.) + z3 * smooth(z0, 150., 200.) + z4 * smooth(z0, 150., 250.);
	z += (z1 * (1. - smooth(z, 0., 60.)));
	return z;
}
