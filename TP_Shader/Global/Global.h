#pragma once

#include "Constants.h"
#include <cmath>
#include <algorithm>

inline bool quadratic(float A, float B, float C, float * t0, float * t1)
{
	float delta = B * B - 4.f * A * C;
	if (delta <= 0.) return false;
	float squaredDelta = sqrtf(delta);
	float q;
	if (B < 0.f) q = -.5f * (B - squaredDelta);
	else q = -.5f * (B + squaredDelta);
	*t0 = q / A;
	*t1 = C / q;
	if (*t0 > *t1) std::swap(*t0, *t1);
	return true;
}
inline float lerp(float t, float v1, float v2)
{
	return (1.f - t) * v1 + t * v2;
}

inline float clamp(float val, float low, float high)
{
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}



inline int mod(int a, int b)
{
	int n = int(a / b);
	a -= n * b;
	if (a < 0) a += b;
	return a;
}

inline float radians(float deg)
{
	return(M_PI / 180.f) * deg;
}

inline float degres(float rad)
{
	return(180.f / M_PI) * rad;
}

inline float randf(float low, float high)
{
	return low + (float)rand() / (float)(RAND_MAX / (high - low));
}
struct ColorRGB
{
	float x, y, z;
	ColorRGB operator*(float f)
	{
		return ColorRGB{ x * f, y * f, z * f };
	}

	ColorRGB operator*(ColorRGB c)
	{
		return ColorRGB{ x * c.x, y * c.y, z * c.z };
	}

	ColorRGB operator/(float f)
	{
		return ColorRGB{ x /f, y /f, z /f};
	}

	ColorRGB operator+(ColorRGB c)
	{
		return ColorRGB{ x + c.x, y + c.y, z + c.z };
	}

	ColorRGB operator-(ColorRGB c)
	{
		return ColorRGB{ x - c.x, y - c.y, z - c.z };
	}

	ColorRGB cclamp(float min, float max)
	{
		return ColorRGB{ clamp(x, min, max), clamp(y, min, max), clamp(z, min, max) };
	}

	ColorRGB applyGamma()
	{
		float p = 1.f / 2.2f;
		return ColorRGB{ powf(x, p), powf(y, p), powf(z, p) };
	}
	ColorRGB toRgb()
	{
		return operator*(255.f);
	}
};

const ColorRGB black = ColorRGB{ .0f, .0f, .0f };
const ColorRGB white = ColorRGB{ 0.75f, 0.75f, 0.75f } *255.f;
const ColorRGB blue = ColorRGB{ .25f, .25f, .75f } *255.f;
const ColorRGB red = ColorRGB{ .75f, .25f, .25f } *255.f;
const ColorRGB yellow = ColorRGB{ .9f, .1f, .9f } *255.f;
const ColorRGB purple = ColorRGB{ .9f, .9f, .1f } *255.f;