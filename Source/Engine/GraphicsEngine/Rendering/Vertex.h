#pragma once  
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <vector>
#include <wrl.h>
using namespace Microsoft::WRL;
// We'll be writing this a lot so easier
// to just typedef it here. 

struct Vertex
{
	Vector4f myPosition;
	Vector4f myColor;
	Vector2f myUV;
	Vector3f myNormal;
	Vector3f myTangent;
	//Vector4<unsigned int> myBoneIds;
	//Vector4f myBoneWeights;

	void AddBoneWeightAndID(float newWeight,unsigned int aId)
	{
		newWeight; aId;
		//for (int i = 0; i < 4; i++)
		//{
		//	if (myBoneWeights[i] == 0.0f)
		//	{
		//		myBoneWeights[i] = newWeight;
		//		myBoneIds[i] = aId;
		//		return;
		//	}
		//}
	}

	Vertex(Vector3f aPosition,
		Vector4f aColor,
		Vector2f aUV,
		Vector3f aNormal,
		Vector3f aTangent,
		Vector4<unsigned int> aBoneIds,
		Vector4f aBoneWeights
	)
	{
		; aBoneIds; aBoneWeights;
		myPosition.x = aPosition.x;
		myPosition.y = aPosition.y;
		myPosition.z = aPosition.z;
		myPosition.w = 1.0f;

		myColor = aColor;
		myUV = aUV;
		myNormal = aNormal;
		myTangent = aTangent;
	}
};

