#pragma once
#include "../InterOp/ShaderInfo.h"


struct ID3D11DeviceChild;
struct ID3D11ShaderReflection;
//Do rewrite, not my structure prb doesnt fit, just need for rhi
class Shader
{
	/*ComPtr<ID3D11DeviceChild> myShaderObject;
	ShaderType myShaderType = ShaderType::VertexShader;*/

	friend class RHI;

	unsigned char* myBlob = nullptr;
	size_t myBlobSize = 0;
	ComPtr<ID3D11DeviceChild> myShaderObject;
	ShaderInfo myShaderInfo{};


public:
	std::wstring myName;

	FORCEINLINE void SetShader(ComPtr<ID3D11DeviceChild> aShader)
	{
		myShaderObject = aShader;
	}

	FORCEINLINE ShaderType GetShaderType() const
	{
		return myShaderInfo.Type;
	}
	FORCEINLINE ComPtr<ID3D11DeviceChild> GetShader() const
	{
		return myShaderObject;
	}
	FORCEINLINE const unsigned char* GetBlob() const
	{
		return myBlob;
	}
	FORCEINLINE size_t GetBlobSize() const
	{
		return myBlobSize;
	}
	FORCEINLINE const ShaderInfo& GetShaderInfo() const
	{
		return myShaderInfo;
	}
};