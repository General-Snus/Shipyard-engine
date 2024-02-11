#pragma once
#include "../InterOp/ShaderInfo.h"

#include <DirectX/directx/d3d12.h>
//Do rewrite, not my structure prb doesnt fit, just need for rhi
class Shader
{
	/*ComPtr<ID3D11DeviceChild> myShaderObject;
	ShaderType myShaderType = ShaderType::VertexShader;*/

	friend class GPU;
	friend class ShipyardShader;

	ComPtr<ID3DBlob> myBlob = nullptr;
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
	FORCEINLINE LPVOID GetBufferPtr()
	{
		return myBlob->GetBufferPointer();
	}
	FORCEINLINE size_t GetBlobSize() const
	{
		return myBlob->GetBufferSize();
	}
	FORCEINLINE const ShaderInfo& GetShaderInfo() const
	{
		return myShaderInfo;
	}
};