#pragma once
#include "Vector.h"
#include "Point.h"
#include "Normals.h"
#include "Ray.h"
#include "BBox.h"
#include <random>
// inline global fct
static std::default_random_engine generator;
static std::uniform_real_distribution<float> distribution(0.0, 1.0);

inline float random_u()
{
	return distribution(generator);
}



// Vector
inline Vector::Vector (const Normals &n) : x(n.x), y(n.y), z(n.z){}

inline Vector operator*(float f, Vector v) { return v * f; };

inline Vector operator/(float f, Vector v) { return v / f; };

inline float dot(const Vector& v1, const Vector& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline float absDot(const Vector& v1, const Vector& v2)
{
	return fabsf(dot(v1, v2));
}


inline Vector cross(const Vector& v1, const Vector& v2)
{
	return Vector((v1.y * v2.z) - (v1.z * v2.y),
		(v1.z * v2.x) - (v1.x * v2.z),
		(v1.x * v2.y) - (v1.y * v2.x));
}

inline Vector normalize(const Vector& v) { return v / v.length(); };

// Point
inline float distance(const Point & p1, const Point & p2) { return (p1 - p2).length(); };

inline float distanceSquared(const Point & p1, const Point & p2) { return (p1 - p2).lengthSquared(); };

// Normals
inline Normals operator*(float f, Normals n) { return n * f; };

inline Normals operator/(float f, Normals n) { return n / f; };

inline Vector operator-(Normals n, Vector v) { return Vector(n.x - v.x, n.y - v.y, n.z - v.z); };
inline Vector operator-(Vector v, Normals n) { return Vector(v.x - n.x, v.y - n.y, v.z - n.z); };
inline float dot(const Normals& n1, const Normals& n2)
{
	return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

inline float dot(const Normals& n, const Vector& v)
{
	return n.x * v.x + n.y * v.y + n.z * v.z;
}

inline float dot(const Vector& v, const Normals& n)
{
	return v.x * n.x + v.y * n.y + v.z * n.z;
}

inline float dot(const Normals& n, const Point& p)
{
	return n.x * p.x + n.y * p.y + n.z * p.z;
}

inline float dot(const Point& p, const Normals& n)
{
	return p.x * n.x + p.y * n.y + p.z * n.z;
}

inline float absDot(const Normals& n1, const Normals& n2)
{
	return fabsf(dot(n1, n2));
}

inline float absDot(const Normals& n, const Vector& v)
{
	return fabsf(dot(n, v));
}


inline float absDot(const Vector& v, const Normals& n)
{
	return fabsf(dot(v, n));
}

inline float absDot(const Normals& n, const Point& p)
{
	return fabsf(dot(n, p));
}

inline float absDot(const Point& p, const Normals& n)
{
	return fabsf(dot(p, n));
}


inline Normals normalize(const Normals& n) { return n / n.length(); };

inline Normals faceForward(const Normals& n, const Vector& v)
{
	return dot(n, v) < 0.f ? n : -n;
}

// BBox
inline BBox unionBBox(const BBox & b, const Point & p)
{
	BBox ret(b);
	ret.pMin.x = std::min(b.pMin.x, p.x);
	ret.pMin.y = std::min(b.pMin.y, p.y);
	ret.pMin.z = std::min(b.pMin.z, p.z);
	ret.pMax.x = std::max(b.pMax.x, p.x);
	ret.pMax.y = std::max(b.pMax.y, p.y);
	ret.pMax.z = std::max(b.pMax.z, p.z);
	return ret;
}

// BBox
inline BBox unionBBox(const BBox & b1, const BBox & b2)
{
	BBox ret;
	ret.pMin.x = std::min(b1.pMin.x, b2.pMin.x);
	ret.pMin.y = std::min(b1.pMin.y, b2.pMin.y);
	ret.pMin.z = std::min(b1.pMin.z, b2.pMin.z);
	ret.pMax.x = std::max(b1.pMax.x, b2.pMax.x);
	ret.pMax.y = std::max(b1.pMax.y, b2.pMax.y);
	ret.pMax.z = std::max(b1.pMax.z, b2.pMax.z);
	return ret;
}

// Lightt

// Reflect the ray i along the normal.
// i should be oriented as "leaving the surface"
inline Vector reflect(Vector i, Normals n)
{
	return n * dot(n, i) * 2.f - i;
}


// Fresnel coeficient of transmission.
// Normal point outside the surface
// ior is n0 / n1 where n0 is inside and n1 is outside
inline float fresnelR(Vector i, Vector n, float ior)
{
	if (dot(n, i) < 0)
		return fresnelR(i, n * -1.f, 1.f / ior);

	float R0 = (ior - 1.f) / (ior + 1.f);
	R0 *= R0;

	return R0 + (1.f - R0) * std::pow(1.f - dot(i, n), 5.f);
}

// compute refraction vector.
// return true if refraction is possible.
// i and n are normalized
// output wo, the refracted vector (normalized)
// n point oitside the surface.
// ior is n00 / n1 where n0 is inside and n1 is outside
//
// i point outside of the surface
inline bool refract(Vector i, Normals n, float ior, Vector &wo)
{
	i = i * -1.f;

	if (dot(n, i) > 0)
	{
		n = n * -1.f;
	}
	else
	{
		ior = 1.f / ior;
	}

	float k = 1.f - ior * ior * (1.f - dot(n, i) * dot(n, i));
	if (k < 0.)
		return false;

	wo = i * ior - n * (ior * dot(n, i) + std::sqrt(k));

	return true;
}

// Monte-Carlo

inline Vector sample_cos(const float u, const float v, const Normals n)
{
	// Ugly: create an ornthogonal base
	Vector basex, basey, basez;

	basez.x = n.x;
	basez.y = n.y;
	basez.z = n.z;
	basey = Vector(n.y, n.z, n.x);

	basex = cross(basez, basey);
	basex = normalize(basex);

	basey = cross(basez, basex);

	// cosinus sampling. Pdf = cosinus
	return  basex * (std::cos(2.f * M_PI * u) * std::sqrt(1.f - v)) +
		basey * (std::sin(2.f * M_PI * u) * std::sqrt(1.f - v)) +
		basez * std::sqrt(v);
}

inline Vector sample_sphere(const float r, const float u, const float v, float &pdf, const Normals normal)
{
	pdf = 1.f / (M_PI * r * r);
	Vector sample_p = sample_cos(u, v, normal);

	float cos = dot(sample_p, normal);

	pdf *= cos;
	return sample_p * r;
} 