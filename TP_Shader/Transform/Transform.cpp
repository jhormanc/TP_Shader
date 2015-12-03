#include "Transform.h"


Transform::Transform()
{
}

Transform::Transform(Mat4x4 m_) : m(m_)
{
}

Transform Transform::operator*(const Transform &t)
{
	return Transform(m * t.m);
}
Transform::~Transform()
{
}
