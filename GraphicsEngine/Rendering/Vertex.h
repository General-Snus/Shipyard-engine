#pragma once
#include <wrl.h>
using namespace Microsoft::WRL;
#include <ThirdParty/Cu/CommonUtills/Vectors.hpp>
#include <d3d11.h>
// We'll be writing this a lot so easier
// to just typedef it here.
typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementList;

struct Vertex
{
	CommonUtilities::Vector4<float> myPosition;
	CommonUtilities::Vector4<float> myColor;
	CommonUtilities::Vector2<float> myUV;
	CommonUtilities::Vector3<float> myNormal;
	CommonUtilities::Vector3<float> myTangent;
	CommonUtilities::Vector4<unsigned int> myBoneIds;
	CommonUtilities::Vector4<float> myBoneWeights;

	 
	Vertex(CommonUtilities::Vector3<float> aPosition,
			CommonUtilities::Vector4<float> aColor,
			CommonUtilities::Vector2<float> aUV,
			CommonUtilities::Vector3<float> aNormal,
			CommonUtilities::Vector3<float> aTangent,
			CommonUtilities::Vector4<unsigned int> aBoneIds,
			CommonUtilities::Vector4<float> aBoneWeights
		)
	{
		myPosition.x = aPosition.x;
		myPosition.y = aPosition.y;
		myPosition.z = aPosition.z;
		myPosition.w = 1.0f;

		myColor = aColor;
		myUV = aUV;
		myNormal = aNormal;
		myTangent = aTangent;
		myBoneIds = aBoneIds; 
		myBoneWeights = aBoneWeights; 
	}
	static const InputElementList InputLayoutDefinition;
	static ComPtr<ID3D11InputLayout> InputLayout;
};

