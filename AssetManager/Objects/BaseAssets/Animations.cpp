#include "AssetManager.pch.h"
#include "Animations.h"

void Animation::Init()
{
	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Animation inAnim;

	if(!std::filesystem::exists(AssetPath))
	{
		assert(false && "Animation file does not exist");
	}

	if(TGA::FBX::Importer::LoadAnimationW(AssetPath,inAnim))
	{
		duration = inAnim.Duration;
		frameRate = inAnim.FramesPerSecond;
		numFrames = static_cast<unsigned int>(inAnim.Frames.size());

		for(size_t i = 0; i < inAnim.Frames.size(); i++)
		{
			Frames.push_back(Frame());
			//Frames.back().myTransforms = std::unordered_map<std::string,CU::Matrix4x4<float>>(); 

			for(auto& ref : inAnim.Frames[i].LocalTransforms)
			{
				CU::Matrix4x4<float> mat;
				mat.SetFromRaw(ref.second.Data);
				Frames.back().myTransforms.emplace(ref.first,mat);
			}
		}
	}
	isLoadedComplete = true;
}

Animation::Animation(const std::filesystem::path& aFilePath) : AssetBase(aFilePath),duration(0),frameRate(0),numFrames(0)
{
	
} 

void Skeleton::Init()
{
	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Mesh inMesh;
	if(!std::filesystem::exists(AssetPath))
	{
		assert(false && "Mesh file does not exist");
	}
	try
	{
		if(TGA::FBX::Importer::LoadMeshW(AssetPath,inMesh))
		{
			const bool hasSkeleton = inMesh.Skeleton.GetRoot();
			if(hasSkeleton)
			{
				for(auto& aBone : inMesh.Skeleton.Bones)
				{
					Bone bone;
					CU::Matrix4x4<float> matrix;
					matrix.SetFromRaw(aBone.BindPoseInverse.Data);
					matrix = CU::Matrix4x4<float>::Transpose(matrix);

					bone.BindPoseInverse = matrix;
					bone.Children = aBone.Children;
					bone.Name = aBone.Name;
					bone.ParentIdx = aBone.ParentIdx;

					myBones.push_back(bone);
				}
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Error: Meshloader failed to load: " << AssetPath << "\n" << e.what() << "\n";
	}
	isLoadedComplete = true;
}

Skeleton::Skeleton(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{

}