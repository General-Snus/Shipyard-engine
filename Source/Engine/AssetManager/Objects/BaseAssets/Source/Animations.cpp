#include "AssetManager.pch.h"
#include "../Animations.h"
 
 

void Animation::Init()
{
	this->duration = 0;
	this->frameRate = 0;
	this->Frames.clear();
	this->numFrames = 0;
	this->isLoadedComplete = false;



	if(!std::filesystem::exists(AssetPath))
	{
		assert(false && "Animation file does not exist");
	}

#if UseTGAImporter == 1
	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Animation inAnim;

	if(TGA::FBX::Importer::LoadAnimationW(AssetPath,inAnim))
	{
		duration = inAnim.Duration;
		frameRate = inAnim.FramesPerSecond;
		numFrames = static_cast<unsigned int>(inAnim.Frames.size());

		for(size_t i = 0; i < inAnim.Frames.size(); i++)
		{
			Frames.push_back(Frame());
			//Frames.back().myTransforms = std::unordered_map<std::string,Matrix4x4<float>>(); 

			for(const auto& ref : inAnim.Frames[i].LocalTransforms)
			{
				Matrix mat;
				mat.SetFromRaw(ref.second.Data); 
				Frames.back().myTransforms.emplace(ref.first,mat);
			}
		}
		isLoadedComplete = true;
	}

#else
	Assimp::Importer importer; 
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS,false);
	const aiScene* scene = importer.ReadFile(AssetPath.string(),(unsigned int)
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType |
		aiProcess_GenBoundingBoxes |
		aiProcess_GlobalScale |
		aiProcess_ConvertToLeftHanded);

	if(scene->HasAnimations())
	{
		if(scene->mNumAnimations > 1)
		{
			AMLogger.Warn("Attempt at loading multiple animation in one file in " + AssetPath.string());
		}
		if(scene->mAnimations[0]->mDuration <= 0)
		{
			AMLogger.Warn("Attempt at loading animation with a duration of zero at " + AssetPath.string());
		}
		duration = scene->mAnimations[0]->mDuration;


		if(scene->mAnimations[0]->mTicksPerSecond == 0)
		{
			AMLogger.Warn("Attempt at loading animation with a framerate of zero at " + AssetPath.string());
		}

		frameRate = static_cast<float>(scene->mAnimations[0]->mTicksPerSecond);
		numFrames = scene->mAnimations[0]->mChannels[0]->mNumPositionKeys;// static_cast<unsigned int>(inAnim.Frames.size());
		Frames.resize(numFrames);

		const unsigned int numAnimatedBone = scene->mAnimations[0]->mNumChannels;
		for(unsigned int i = 0; i < numAnimatedBone; i++)
		{ 
			const aiNodeAnim* channel = scene->mAnimations[0]->mChannels[i];
			std::string name = channel->mNodeName.C_Str();
			for(unsigned int j = 0; j < numFrames; j++)
			{ 
				Matrix mat;
				const Vector3f scale = {
					channel->mScalingKeys[j].mValue.x,
					channel->mScalingKeys[j].mValue.y,
					channel->mScalingKeys[j].mValue.z
				};
				mat *= Matrix::CreateScaleMatrix(scale);

				const Vector3f rotation = {
					channel->mRotationKeys[j].mValue.x,
					channel->mRotationKeys[j].mValue.y,
					channel->mRotationKeys[j].mValue.z
				};
				mat *= Matrix::CreateRotationMatrix(rotation);

				const Vector3f transform = {
					channel->mPositionKeys[j].mValue.x,
					channel->mPositionKeys[j].mValue.y,
					channel->mPositionKeys[j].mValue.z
				};
				mat *= Matrix::CreateTranslationMatrix(transform);
				auto frameNumber = static_cast<int>(channel->mPositionKeys[j].mTime);

				if(const size_t pos = name.find_last_of(':'); pos != std::string::npos)
				{
					name  = name.substr(pos + 1);
				}

				Frames[frameNumber].myTransforms.emplace(name,mat);
			}

		}
		isLoadedComplete = true;
	}
	else
	{
		AMLogger.Err("Failed to load animation from " + AssetPath.string());
	}
#endif


}

Animation::Animation(const std::filesystem::path& aFilePath) : AssetBase(aFilePath),duration(0),frameRate(0),numFrames(0)
{

}


void Skeleton::Init()
{
	if(!std::filesystem::exists(AssetPath))
	{
		assert(false && "Mesh file does not exist");
	}
#if UseTGAImporter == 1
	TGA::FBX::Importer::InitImporter();
	TGA::FBX::Mesh inMesh;
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
					Matrix matrix;
					matrix.SetFromRaw(aBone.BindPoseInverse.Data);
					matrix = Matrix::Transpose(matrix); 

					bone.BindPoseInverse = matrix;
					bone.Children = aBone.Children;
					bone.Name = aBone.Name;
					bone.ParentIdx = aBone.ParentIdx;

					myBones.push_back(bone);
				}
			}
			isLoadedComplete = true;
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Error: Meshloader failed to load: " << AssetPath << "\n" << e.what() << "\n";
	}
#else
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	importer.SetPropertyBool(AI_CONFIG_FBX_USE_SKELETON_BONE_CONTAINER,true);
	const aiScene* scene = importer.ReadFile(AssetPath.string(),(unsigned int)
		aiProcess_GlobalScale |
		aiProcess_PopulateArmatureData | aiProcess_ValidateDataStructure 
	);

	if(scene->HasMeshes())
	{ 
		myName = scene->mMeshes[0]->mName.C_Str();
		//myName = scene->mSkeletons[0]->mName.C_Str();
		for(unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			for(unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
			{
				unsigned int boneIndex = 0;
				aiNode* skeletonBone = scene->mMeshes[i]->mBones[j]->mNode;
				std::string name = skeletonBone->mName.C_Str();

				if(!BoneNameToIndex.contains(name))
				{
					boneIndex = static_cast<unsigned int>(myBones.size());
					Bone bone;
					bone.Name = name;
					myBones.push_back(bone);
				}
				else
				{
					boneIndex = BoneNameToIndex[name];
				}
				BoneNameToIndex[name] = boneIndex;
				Matrix matrix(scene->mMeshes[i]->mBones[j]->mOffsetMatrix);
				matrix = Matrix::Transpose(matrix);
				myBones[boneIndex].BindPoseInverse = matrix;
			}

			for(unsigned int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
			{
				aiNode* skeletonBone = scene->mMeshes[i]->mBones[j]->mNode;
				std::string name = skeletonBone->mName.C_Str();

				if(BoneNameToIndex.contains(skeletonBone->mParent->mName.C_Str())) // hidden nodes sits above the skeleton stopping me from checking if root
				{
					std::string rootName = skeletonBone->mParent->mName.C_Str();
					myBones[BoneNameToIndex[name]].ParentIdx = BoneNameToIndex[rootName];
				}
				for(unsigned int k = 0; k < skeletonBone->mNumChildren; k++)
				{
					std::string childName = skeletonBone->mChildren[k]->mName.C_Str();
					if(BoneNameToIndex.contains(childName))
					{
						myBones[BoneNameToIndex[name]].Children.push_back(BoneNameToIndex[childName]);
					}
				}
			}
		}

		isLoadedComplete = true;
	}
	else
	{
		AMLogger.Err("Failed to load skeleton from " + AssetPath.string());
	}
#endif
}

Skeleton::Skeleton(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{

}