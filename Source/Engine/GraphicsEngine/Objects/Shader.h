#pragma once
#include "../InterOp/ShaderInfo.h"


//Do rewrite, not my structure prb doesnt fit, just need for rhi
class Shader
{
	/*ComPtr<ID3D11DeviceChild> myShaderObject;
	ShaderType myShaderType = ShaderType::VertexShader;*/

	friend class GPU;

	ComPtr<ID3DBlob> myBlob = nullptr;
	size_t myBlobSize = 0;
	ShaderInfo myShaderInfo{};


public:
	std::wstring myName;

	FORCEINLINE void SetShader(ComPtr<ID3DBlob> aShader)
	{
		myBlob = aShader;
	}

	FORCEINLINE ShaderType GetShaderType() const
	{
		return myShaderInfo.Type;
	}
	FORCEINLINE const ID3DBlob* GetBlob() const
	{
		return myBlob.Get();
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