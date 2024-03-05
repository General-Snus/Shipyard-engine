#include "AssetManager.pch.h"
#include "../ShipyardShader.h"

#include <d3dcompiler.h> 

#include "DirectX/Shipyard/Helpers.h" 

ShipyardShader::ShipyardShader(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	shaderObject.myName = aFilePath.filename();
}

void ShipyardShader::Init()
{
	Helpers::ThrowIfFailed(D3DReadFileToBlob(AssetPath.wstring().c_str(),&shaderObject.myBlob));
}

Shader& ShipyardShader::GetShader()
{
	return shaderObject;
}