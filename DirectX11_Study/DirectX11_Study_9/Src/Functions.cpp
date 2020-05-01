#include "Functions.h"

#include <cmath>

#define PI 3.14159265358979

// Vector3Œn

XMFLOAT3 normalize3(const XMFLOAT3 & vector3)
{
	float len = length(vector3);

	if (len < 0.000000000000001f) return XMFLOAT3(0.0f, 0.0f, 0.0f);

	return XMFLOAT3(
		vector3.x / len,
		vector3.y / len,
		vector3.z / len
	);
}

float dot(const XMFLOAT3 & vec1, const XMFLOAT3 & vec2)
{
	return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

XMFLOAT3 cross(const XMFLOAT3 & vec1, const XMFLOAT3 & vec2)
{
	return XMFLOAT3(
		vec1.y * vec2.z - vec1.z * vec2.y,
		vec1.z * vec2.x - vec1.x * vec2.z,
		vec1.x * vec2.y - vec1.y * vec2.x);
}

float length(const XMFLOAT3 & vec)
{
	return std::sqrt(dot(vec, vec));
}

XMFLOAT3 lerp(XMFLOAT3 vec1, XMFLOAT3 vec2, float t)
{
	if (t > 1.0f)
	{
		t = 1.0f;
	}
	else if (t < 0.0f)
	{
		t = 0.0f;
	}

	return XMFLOAT3(
		vec1.x + ((vec2.x - vec1.x) * t),
		vec1.y + ((vec2.y - vec1.y) * t),
		vec1.z + ((vec2.z - vec1.z) * t)
	);
}

// Matrix4Œn

XMFLOAT4X4 createFromQuaternion(const XMFLOAT4 & rotate)
{
	float xx = rotate.x * rotate.x * 2.0f;
	float yy = rotate.y * rotate.y * 2.0f;
	float zz = rotate.z * rotate.z * 2.0f;
	float xy = rotate.x * rotate.y * 2.0f;
	float xz = rotate.x * rotate.z * 2.0f;
	float yz = rotate.y * rotate.z * 2.0f;
	float wx = rotate.w * rotate.x * 2.0f;
	float wy = rotate.w * rotate.y * 2.0f;
	float wz = rotate.w * rotate.z * 2.0f;

	return XMFLOAT4X4(
		1.0f - yy - zz, xy + wz, xz - wy, 0.0f,
		xy - wz, 1.0f - xx - zz, yz + wx, 0.0f,
		xz + wy, yz - wx, 1.0f - xx - yy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

XMFLOAT4X4 createTRS(const XMFLOAT3 & translation, const XMFLOAT4 & rotate, const XMFLOAT3 & scale)
{
	XMFLOAT4X4 result = createFromQuaternion(rotate);

	result.m[0][0] *= scale.x;       result.m[0][1] *= scale.x;       result.m[0][2] *= scale.x;
	result.m[1][0] *= scale.y;       result.m[1][1] *= scale.y;       result.m[1][2] *= scale.y;
	result.m[2][0] *= scale.z;       result.m[2][1] *= scale.z;       result.m[2][2] *= scale.z;
	result.m[3][0] = translation.x;  result.m[3][1] = translation.y;  result.m[3][2] = translation.z;

	return result;
}

XMFLOAT4X4 createPerspectiveFieldOfViewLH(float fov, float aspect, float zn, float zf)
{
	fov = fov * (PI / 180.0f);
	float h = 1.0f / std::tan(fov / 2.0f);
	float w = h / aspect;

	return XMFLOAT4X4(
		w, 0.0f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, zf / (zf - zn), 1.0f,
		0.0f, 0.0f, (-zn * zf) / (zf - zn), 0.0f
	);
}

XMFLOAT4X4 createLookAtLH(const XMFLOAT3 & position, const XMFLOAT3 & target, const XMFLOAT3 & up)
{
	XMFLOAT3 tar = target;
	XMFLOAT3 z = normalize3(tar - position);
	XMFLOAT3 x = normalize3(cross(up, z));
	XMFLOAT3 y = cross(z, x);

	return XMFLOAT4X4(
		x.x, y.x, z.x, 0.0f,
		x.y, y.y, z.y, 0.0f,
		x.z, y.z, z.z, 0.0f,
		-dot(position, x), -dot(position, y), -dot(position, z), 1.0f
	);
}

XMFLOAT4X4 createRotationY(float angle)
{
	float sin = std::sin(angle);
	float cos = std::cos(angle);

	return XMFLOAT4X4(
		cos, 0.0f, -sin, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin, 0.0f, cos, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

// QuaternionŒn

XMFLOAT4 slerp(const XMFLOAT4 & val1, const XMFLOAT4 & val2, float t)
{
	float cos = calculateDot(val1, val2);

	float sign = 1.0f;

	if (cos < 0.0f)
	{
		cos = -cos;
		sign = -1.0f;
	}

	float t0 = 1.0f - t;
	float t1 = t;

	if (cos < 0.999f)
	{
		float theta = std::acos(cos);
		t0 = std::sin(theta * t0) / std::sin(theta);
		t1 = std::sin(theta * t1) / std::sin(theta);
	}

	XMFLOAT4 temp1 = val1 * t0;
	XMFLOAT4 temp2 = val2 * sign * t1;
	XMFLOAT4 temp3 = temp1 + temp2;

	return normalize4(temp3);
}

float calculateDot(const XMFLOAT4 & val1, const XMFLOAT4 & val2)
{
	return (val1.x * val2.x) + (val1.y * val2.y) + (val1.z * val2.z) + (val1.w * val2.w);
}

XMFLOAT4 normalize4(const XMFLOAT4 & q)
{
	float len = calculateLength(q);

	if (len > 0.0000001f)
	{
		return XMFLOAT4(
			q.x / len,
			q.y / len,
			q.z / len,
			q.w / len
		);
	}
	return XMFLOAT4(q.x, q.y, q.z, q.w);
}

float calculateLength(const XMFLOAT4& q)
{
	return std::sqrt(calculateDot(q, q));
}
