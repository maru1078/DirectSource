#pragma once

#include <DirectXMath.h>

using namespace DirectX;

XMFLOAT3 operator + (XMFLOAT3& v1, const XMFLOAT3& v2);
XMFLOAT3 operator - (XMFLOAT3& v1, const XMFLOAT3& v2);
XMFLOAT3 operator - (const XMFLOAT3& v);
XMFLOAT3 operator * (XMFLOAT3& v, float t);

XMFLOAT4X4 operator * (const XMFLOAT4X4& mat1, const XMFLOAT4X4& mat2);

XMFLOAT4 operator + (XMFLOAT4 & q1, const XMFLOAT4& q2);
XMFLOAT4 operator * (const XMFLOAT4& q, float t);