#include "Operators.h"

XMFLOAT3 operator+(XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;

	return v1;
}

XMFLOAT3 operator-(XMFLOAT3 & v1, const XMFLOAT3 & v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;

	return v1;
}

XMFLOAT3 operator-(const XMFLOAT3 & v)
{
	return XMFLOAT3(-v.x, -v.y, -v.z);
}

XMFLOAT3 operator*(XMFLOAT3 & v, float t)
{
	v.x *= t;
	v.y *= t;
	v.z *= t;

	return v;
}

XMFLOAT4X4 operator*(const XMFLOAT4X4 & mat1, const XMFLOAT4X4 & mat2)
{
	XMFLOAT4X4 result = XMFLOAT4X4(
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] += mat1.m[i][k] * mat2.m[k][j];
			}
		}
	}
	return result;
}

XMFLOAT4 operator+(XMFLOAT4 & q1, const XMFLOAT4 & q2)
{
	q1.x += q2.x;
	q1.y += q2.y;
	q1.z += q2.z;
	q1.w += q2.w;

	return q1;
}

XMFLOAT4 operator*(const XMFLOAT4 & q, float t)
{
	XMFLOAT4 result;
	result.x = q.x * t;
	result.y = q.y * t;
	result.z = q.z * t;
	result.w = q.w * t;

	return result;
}
