#include "Noise.h"


static double unit = 1.0f / sqrt ( 2.0f );

static double grad2[][2] = { { unit, unit }, { -unit, unit }, { unit, -unit }, { -unit, -unit },
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

// To remove the need for index wrapping, double the permutation table length
static int perm[512];

// This method is a *lot* faster than using (int)Math.floor(x)
int Noise::fastfloor ( double x ) {
	return x>0 ? ( int ) x : ( int ) x - 1;
}

double Noise::dot ( int g[], double x, double y ) {
	return g[0] * x + g[1] * y;
}

// Renvoi une valeur aleatoire entre 0 et 1 a partir de x.
double Noise::hash ( double x ) {
	return ( 1.0 + ( sin ( sin ( x ) * rand ( ) ) ) ) / 2.0;
}

double Noise::ridge ( const double z, const double zr ) {
	if ( z < zr )
		return z;
	return 2 * zr - z;
}

Vector Noise::warp ( const Vector p, const double c, const double f ) {
	return p + Vector ( simplex ( f * p.x, f * p.y ), simplex ( f * ( p.x * cos ( 30.0f ) - p.y * sin ( 30.0f ) ) + 10, f * ( p.x * sin ( 30.0f ) + p.y * cos ( 30.0f ) ) + 10 ), 0 ) * c;
}

double Noise::smooth ( const double z, const double zmin, const double zmax ) {
	if ( z < zmin )
		return 0;

	if ( z > zmax )
		return 1;

	double t = ( z - zmin ) / ( zmax - zmin );

	return 1 - ( 1 - t * t ) * ( 1 - t * t );
}

// 2D Simplex noise
double Noise::simplex ( double xin, double yin ) {
	for ( int i = 0; i<512; i++ ) perm[i] = p[i & 255];

	double n0, n1, n2; // Noise contributions from the three corners
	// Skew the input space to determine which Noise cell we're in
	double F2 = 0.5 * ( sqrt ( 3.0 ) - 1.0 );
	double s = ( xin + yin ) * F2; // Hairy factor for 2D
	int i = fastfloor ( xin + s );
	int j = fastfloor ( yin + s );
	double G2 = ( 3.0 - sqrt ( 3.0 ) ) / 6.0;
	double t = ( i + j ) * G2;
	double X0 = i - t; // Unskew the cell origin back to (x,y) space
	double Y0 = j - t;
	double x0 = xin - X0; // The x,y distances from the cell origin
	double y0 = yin - Y0;

	// For the 2D case, the Noise shape is an equilateral triangle.
	// Determine which Noise we are in.
	int i1, j1; // Offsets for second (middle) corner of Noise in (i,j) coords
	if ( x0  > y0 ) {
		i1 = 1;
		j1 = 0;
	} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else {
		i1 = 0;
		j1 = 1;
	} // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	double y2 = y0 - 1.0 + 2.0 * G2;

	// Work out the hashed gradient indices of the three Noise corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0 * x0 - y0 * y0;
	if ( t0 < 0 )
		n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot ( grad3[gi0], x0, y0 ); // (x,y) of grad3 used for 2D gradient
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if ( t1 < 0 )
		n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot ( grad3[gi1], x1, y1 );
	}

	double t2 = 0.5 - x2 * x2 - y2 * y2;
	if ( t2 < 0 )
		n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot ( grad3[gi2], x2, y2 );
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * ( n0 + n1 + n2 );
}

// Perlin noise
// Renvoie un double compris entre -1 et 1
double Noise::perlin2D ( double x, double y, const double res_x, const double res_y ) {
	double tempX, tempY;
	int x0, y0, ii, jj, gi0, gi1, gi2, gi3;

	double tmp, s, t, u, v, Cx, Cy, Li1, Li2;

	//Adapter pour la résolution
	x /= res_x;
	y /= res_y;

	//On récupère les positions de la grille associée à (x,y)
	x0 = ( int ) ( x );
	y0 = ( int ) ( y );

	//Masquage
	ii = x0 & 255;
	jj = y0 & 255;

	//Pour récupérer les vecteurs
	gi0 = p[ii + p[jj]] % 8;
	gi1 = p[ii + 1 + p[jj]] % 8;
	gi2 = p[ii + p[jj + 1]] % 8;
	gi3 = p[ii + 1 + p[jj + 1]] % 8;

	//on récupère les vecteurs et on pondère
	tempX = x - x0;
	tempY = y - y0;
	s = grad2[gi0][0] * tempX + grad2[gi0][1] * tempY;

	tempX = x - ( x0 + 1 );
	tempY = y - y0;
	t = grad2[gi1][0] * tempX + grad2[gi1][1] * tempY;

	tempX = x - x0;
	tempY = y - ( y0 + 1 );
	u = grad2[gi2][0] * tempX + grad2[gi2][1] * tempY;

	tempX = x - ( x0 + 1 );
	tempY = y - ( y0 + 1 );
	v = grad2[gi3][0] * tempX + grad2[gi3][1] * tempY;


	//Lissage
	tmp = x - x0;
	Cx = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

	Li1 = s + Cx*( t - s );
	Li2 = u + Cx*( v - u );

	tmp = y - y0;
	Cy = 3 * tmp * tmp - 2 * tmp * tmp * tmp;

	return Li1 + Cy*( Li2 - Li1 );
}

double Noise::noise ( double x, double y ) {
	Vector w = warp ( Vector ( x, y, 0 ), 5, 1 / 50.0 );
	double tmp;
	tmp = ( simplex ( w.x / 500, w.y / 500 ) + 1 ) * 0.5;

	double z0 = ridge ( 200 * tmp, 180 );

	tmp = ( simplex ( w.x / 200, w.y / 200 ) + 1 ) * 0.5;

	double z1 = 50 * tmp; //ridge(50 * tmp, 15);

	tmp = ( simplex ( w.x / 50, w.y / 50 ) + 1 ) * 0.5;

	double z2 = 15 * tmp; //ridge(15 * tmp, 5);

	double z = z0 * smooth ( z0, 50, 200 ) + z1 * smooth ( z0, 50, 200 ) + z2 * smooth ( z0, 50, 200 );
	return z;
}
