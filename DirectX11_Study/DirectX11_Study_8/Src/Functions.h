#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "Operators.h"

// Vector3ån
XMFLOAT3 normalize3(const XMFLOAT3& vector3);
float dot(const XMFLOAT3& vec1, const XMFLOAT3& vec2);
XMFLOAT3 cross(const XMFLOAT3& vec1, const XMFLOAT3& vec2);
float length(const XMFLOAT3& vec);
XMFLOAT3 lerp(XMFLOAT3 vec1, XMFLOAT3 vec2, float t);

// Matrix4ånÅiXMFLOAT4X4Åj
XMFLOAT4X4 createFromQuaternion(const XMFLOAT4& rotate);
XMFLOAT4X4 createTRS(const XMFLOAT3& translation, const XMFLOAT4& rotate, const XMFLOAT3& scale);
XMFLOAT4X4 createPerspectiveFieldOfViewLH(float fov, float aspect, float zn, float zf);
XMFLOAT4X4 createLookAtLH(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up);
XMFLOAT4X4 createRotationY(float angle);

// QuaternionånÅiXMFLOAT4Åj
XMFLOAT4 slerp(const XMFLOAT4& val1, const XMFLOAT4& val2, float t);
float calculateDot(const XMFLOAT4& val1, const XMFLOAT4& val2);
XMFLOAT4 normalize4(const XMFLOAT4& q);
float calculateLength(const XMFLOAT4& q);
