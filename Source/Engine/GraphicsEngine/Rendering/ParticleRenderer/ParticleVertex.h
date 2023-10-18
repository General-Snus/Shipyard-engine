#pragma once

struct Particlevertex
{
	Vector4f Position = {0,0,0,1};
	Vector4f Color = {0,0,0,0};
	Vector3f Velocity = {0,0,0};
	Vector3f Scale = {1,1,1};
	float Lifetime = 0;

	static const InputElementList InputLayoutDefinition;
	static ComPtr<ID3D11InputLayout> InputLayout;
};
