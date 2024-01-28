#pragma once
#include <d3d11.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <wrl.h>
using namespace Microsoft::WRL;
// We'll be writing this a lot so easier
// to just typedef it here.
typedef std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementList;

struct Particlevertex
{
	Vector4f Position = { 0,0,0,1 };
	Vector4f Color = { 0,0,0,0 };
	Vector3f Velocity = { 0,0,0 };
	Vector3f Scale = { 1,1,1 };
	float Lifetime = 0;

	static const InputElementList InputLayoutDefinition;
	static ComPtr<ID3D11InputLayout> InputLayout;
};
