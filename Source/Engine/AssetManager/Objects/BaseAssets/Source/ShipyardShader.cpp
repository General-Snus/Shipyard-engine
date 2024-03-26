#include "AssetManager.pch.h"

#include <d3dcompiler.h>
#include "../ShipyardShader.h"
#include "DirectX/Shipyard/Helpers.h" 

ShipyardShader::ShipyardShader(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	m_ShaderName = aFilePath.filename();
}

void ShipyardShader::Init()
{
	isBeingLoaded = true;

	if (!std::filesystem::exists(AssetPath))
	{
		Logger::Warn("Failed to load shader at: " + AssetPath.string());
		isBeingLoaded = false;
		isLoadedComplete = true;
		return;
	}

	Helpers::ThrowIfFailed(D3DReadFileToBlob(AssetPath.wstring().c_str(),&myBlob));

	isBeingLoaded = false;
	isLoadedComplete = true;
}

void ShipyardShader::SetShader(const ComPtr<ID3DBlob>& aShader)
{
	myBlob = aShader;
}

ShaderType ShipyardShader::GetShaderType() const
{
	return myShaderInfo.Type;
}
ID3DBlob* ShipyardShader::GetBlob() const
{
	return myBlob.Get();
}
LPVOID ShipyardShader::GetBufferPtr()
{
	return myBlob->GetBufferPointer();
}
size_t ShipyardShader::GetBlobSize() const
{
	return myBlob->GetBufferSize();
}
const ShaderInfo& ShipyardShader::GetShaderInfo() const
{
	return myShaderInfo;
}