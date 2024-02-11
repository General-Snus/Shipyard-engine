#pragma once

#include <Engine/GraphicsEngine/Objects/Shader.h> 
#include "BaseAsset.h" // fuck you for bad naming

class ShipyardShader : public AssetBase  // <--- 
{
public:
	ShipyardShader(const std::filesystem::path& aFilePath);
	void Init() override;
	Shader& GetShader();
private:
	Shader shaderObject;
};

