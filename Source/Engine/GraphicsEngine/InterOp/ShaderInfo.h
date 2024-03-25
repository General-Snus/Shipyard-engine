#pragma once
#include <string>
#include <unordered_map>
#include <wrl.h>
using namespace Microsoft::WRL;

enum class ShaderType : unsigned
{
	PixelShader = 0,
	VertexShader = 1,
	GeometryShader = 2,
};

enum class TextureType : unsigned
{
	Unknown,
	Texture1D,
	Texture2D,
	Texture3D
};

struct ShaderInfo
{
	struct VariableInfo
	{
		std::wstring Name;
		std::wstring Type;
		size_t Size{};
		size_t Offset{};
	};

	struct ConstantBufferInfo
	{
		std::wstring Name;
		size_t Size{};
		unsigned Slot{};
		std::vector<VariableInfo> Variables;
		std::unordered_map<std::wstring,size_t> VariableNameToIndex;
	};

	struct SamplerInfo
	{
		std::wstring Name;
		unsigned Slot{};
	};

	struct TextureInfo
	{
		std::wstring Name;
		unsigned Slot{};
		TextureType Type;
	};

	std::unordered_map<std::wstring,size_t> ConstantBufferNameToIndex;
	std::vector<ConstantBufferInfo> ConstantBuffers;

	std::unordered_map<std::wstring,size_t> SamplerNameToIndex;
	std::vector<SamplerInfo> Samplers;

	std::unordered_map<std::wstring,size_t> TextureNameToIndex;
	std::vector<TextureInfo> Textures;

	unsigned InstructionCount = 0;
	ShaderType Type;
};