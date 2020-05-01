#pragma once

#include "Mesh.h"
#include "Animation.h"
#include "Skeleton.h"
#include <DirectXMath.h>

using namespace DirectX;

class SkinnedMesh
{
public:

	class Shader
		: public Mesh::Shader
	{
	public:

		virtual ~Shader() {};
		virtual void boneMatrices(int size, const XMFLOAT4X4 matrices[]) = 0;
	};

public:

	SkinnedMesh(Mesh* mesh, Skeleton* skeleton, Animation* animation);

	void calculate(const XMFLOAT4X4& world, float frameNo);
	void draw(SkinnedMesh::Shader& shader) const;

private:

	static const unsigned int BONE_MAX{ 256 };

	const Mesh* m_mesh{ nullptr };
	const Skeleton* m_skeleton{ nullptr };
	const Animation* m_animation{ nullptr };
	XMFLOAT4X4 m_localMatrices[BONE_MAX];
	XMFLOAT4X4 m_worldMatrices[BONE_MAX];
	XMFLOAT4X4 m_skinnedMatrices[BONE_MAX];
};
