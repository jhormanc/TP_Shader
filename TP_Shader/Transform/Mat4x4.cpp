#include "Mat4x4.h"

Mat4x4::Mat4x4()
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = 0; j < 4; ++j)
		{
			if (i == j)
				m[i][j] = 1.f;
			else
				m[i][j] = 0.f;
		}
	}
}

Mat4x4::Mat4x4(float mat[4][4])
{
	for (unsigned int i = 0; i < 4; ++i)
	{
		for (unsigned int j = 0; j < 4; ++j)
		{
			m[i][j] = mat[i][j];
		}
	}
}


Mat4x4::Mat4x4(float t00_, float t01_, float t02_, float t03_,
	float t10_, float t11_, float t12_, float t13_,
	float t20_, float t21_, float t22_, float t23_,
	float t30_, float t31_, float t32_, float t33_) 
	
{
	m[0][0] = t00_; m[0][1] = t01_; m[0][2] = t02_; m[0][3] = t03_,
		m[1][0] = t10_; m[1][1] = t11_; m[1][2] = t12_; m[1][3] = t13_;
	m[2][0] = t20_; m[2][1] = t21_; m[2][2] = t22_; m[2][3] = t23_;
	m[3][0] = t30_; m[3][1] = t31_; m[3][2] = t32_; m[3][3] = t33_;
} 

Mat4x4::~Mat4x4()
{

}