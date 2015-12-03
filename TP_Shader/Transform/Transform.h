#pragma once
#include"Mat4x4.h"
#include "..\Geometry\Geometry.h"
class Transform
{
public:
	Mat4x4 m;
	Transform();
	Transform(Mat4x4 m_);

	Transform operator*(const Transform &t);
	static Transform translate(const Vector &delta)
	{
		return Transform(Mat4x4(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
								0.f, 0.f, 1.f, 0.f, 
								delta.x, delta.y, delta.z, 1.f));
	}

	static Transform scale(float x, float y, float z)
	{
		return Transform(Mat4x4(x, 0.f, 0.f, 0.f,
								0.f, y, 0.f, 0.f,
								0.f, 0.f, z, 0.f,
								0.f, 0.f, 0.f, 1.f));
	}

	static Transform lookAt(const Point& pos, const Point& look, const Vector& up)
	{
		Mat4x4 Matrix;
		/*
		Vector dir = normalize(look - pos);
		Vector left = normalize(cross(normalize(up), dir));
		Vector newUp = cross(dir, left);
		return Transform(Mat4x4(left.x, newUp.x, dir.x, pos.x,
							left.y, newUp.y, dir.y, pos.y,
							left.z, newUp.z, dir.z, pos.z,
							0.f, 0.f, 0.f, 1.f)); */

		Vector Z = pos - look;
		Z = normalize(Z);
		Vector Y = up;
		Vector X = cross(Y, Z);
		Y = cross(Z, X);
		Y = normalize(Y);
		X = normalize(X);
		Vector eye(pos.x, pos.y, pos.z);
		Matrix.m[0][0] = X.x;
		Matrix.m[1][0] = X.y;
		Matrix.m[2][0] = X.z;
		Matrix.m[3][0] = -dot(X, eye);
		Matrix.m[0][1] = Y.x;
		Matrix.m[1][1] = Y.y;
		Matrix.m[2][1] = Y.z;
		Matrix.m[3][1] = -dot(Y, eye);
		Matrix.m[0][2] = Z.x;
		Matrix.m[1][2] = Z.y;
		Matrix.m[2][2] = Z.z;
		Matrix.m[3][2] = -dot(Z, eye);
		Matrix.m[0][3] = 0;
		Matrix.m[1][3] = 0;
		Matrix.m[2][3] = 0;
		Matrix.m[3][3] = 1.0f;
		return Matrix;
	}

	static Transform perspective(float fov, float n, float f)
	{
		Mat4x4 persp(1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, -(f + n) / (f - n), -1.f,
			0.f, 0.f, - 2* (f + n) / (f - n), 0.f);
		float invTanAng = 1.f / tanf(fov / 2.f);
		return scale(invTanAng, invTanAng, 1.f) * Transform(persp);
	}
	~Transform();
};

