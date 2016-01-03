#pragma once

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
		return ColorRGB{ x / f, y / f, z / f };
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
		return ColorRGB{ powf(x / 255.f, p), powf(y / 255.f, p), powf(z / 255.f, p)} * 255.f;
	}

	ColorRGB toRgb()
	{
		return operator*(255.f);
	}

	bool isWhite() const
	{
		return (((int)x) >= 255 && ((int)y) >= 255 && ((int)z) >= 255);
	}

	inline float clamp(float val, float low, float high)
	{
		if (val < low) return low;
		else if (val > high) return high;
		else return val;
	}
};