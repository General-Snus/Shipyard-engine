#pragma once 
#include <d3d12.h>
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
		//myBoneIds = aBoneIds;
		//myBoneWeights = aBoneWeights;
	}
	//static const InputElementList InstancedInputLayoutDefinition;
	//static ComPtr<ID3D11InputLayout> InputLayout;
	//static ComPtr<ID3D11InputLayout> InstancedInputLayout;
	static inline const std::vector<D3D12_INPUT_ELEMENT_DESC> InputLayoutDefinition =
	{
		{ "POSITION",		0,    DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",			0,    DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV",				0,    DXGI_FORMAT_R32G32_FLOAT,			0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0,    DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",		0,    DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "BONEIDS",		0,    DXGI_FORMAT_R32G32B32A32_UINT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "BONEWEIGHTS",	0,    DXGI_FORMAT_R32G32B32A32_FLOAT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	   { "WORLD",			0,    DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 },
	   { "WORLD",			1,    DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	   { "WORLD",			2,    DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1},
	   { "WORLD",			3,    DXGI_FORMAT_R32G32B32A32_FLOAT,   1, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1}
	   //{ "ANIMATIONFRAME",	0,    DXGI_FORMAT_R32G32B32A32_UINT,				0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 0 }
	};
};

