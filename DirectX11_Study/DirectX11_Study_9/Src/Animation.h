#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class Animation
{
public:

	struct KeyFrame
	{
		float frameNo;
		XMFLOAT3 scale;
		XMFLOAT4 rotate;
		XMFLOAT3 position;
		XMFLOAT4X4 matrix() const;
	};

public:

	Animation();

	Animation::KeyFrame getKeyFrame(const std::string& boneName, float frameNo) const;
	void clear();
	void load(const std::string& fileName);
	float endFrame() const;

private:

	std::unordered_map<std::string, std::vector<KeyFrame>> m_boneKeyFrames;
};
