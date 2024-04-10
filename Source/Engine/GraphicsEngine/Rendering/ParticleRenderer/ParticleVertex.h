#pragma once 
#include <DirectX/directx/d3d12.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <wrl.h>
using namespace Microsoft::WRL;
// We'll be writing this a lot so easier
// to just typedef it here. 
struct Particlevertex
{
	Vector4f Position = { 0,0,0,1 };
	Vector4f Color = { 0,0,0,0 };
	Vector3f Velocity = { 0,0,0 };
	Vector3f Scale = { 1,1,1 };
	float Lifetime = 0;

	static inline const D3D12_INPUT_ELEMENT_DESC InputLayoutDefinition[] =
	{
		{ "POSITION",		0,    DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",			0,    DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "VELOCITY",		0,    DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "SCALE",			0,    DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "LIFETIME",		0,    DXGI_FORMAT_R32_FLOAT,			0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
};