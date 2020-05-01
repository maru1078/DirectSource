#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class Animation;

class Skeleton
{
public:

	struct Bone
	{
		char name[32];
		int id;
		int parentId;
		XMFLOAT4X4 transform;
		XMFLOAT4X4 inverse;
	};

public:

	Skeleton() = default;

	std::size_t size() const;
	void calculateLocalMatrices(const Animation& animation, float frameNo, XMFLOAT4X4 local[]) const;
	void calculateWorldMatrices(const XMFLOAT4X4& parent, const XMFLOAT4X4 local[], XMFLOAT4X4 world[]) const;
	void calculateSkinnedMatrices(const XMFLOAT4X4 world[], XMFLOAT4X4 skinned[]) const;
	void clear();
	void load(const std::string& fileName);

private:

	std::vector<Bone> m_bones;
};