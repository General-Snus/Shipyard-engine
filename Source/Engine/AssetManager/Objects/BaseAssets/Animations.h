#pragma once
#include "BaseAsset.h"
struct Bone
{
	Matrix BindPoseInverse;
	int ParentIdx = -1;
	std::string Name;
	std::vector<unsigned> Children;
};


struct Frame
{
	std::unordered_map<std::string,Matrix > myTransforms;
};
class Skeleton : public AssetBase
{
public:
	void Init() override;
	Skeleton(const std::filesystem::path& aFilePath);
	std::vector<Bone> myBones;
	std::string myName;
	std::unordered_map<std::string,unsigned int> BoneNameToIndex;
	const Bone* GetRoot() const { if (!myBones.empty()) { return &myBones[0]; } return nullptr; }
};

class Animation : public AssetBase
{
public:
	void Init() override;
	Animation(const std::filesystem::path& aFilePath);
	Animation() = delete; // Create a generic cube 
private:
	friend class cAnimator;
	double duration;
	float frameRate;
	unsigned int numFrames;
	std::vector<Frame> Frames;
};
