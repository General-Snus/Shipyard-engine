#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>

class GfxCmd_SetLightBuffer : public GraphicCommandBase
{
	std::vector<std::pair< DirectionalLight*,Texture*>> dirLight;
	std::vector<std::pair< PointLight ,Texture*>> pointLight; //REFACTOR pointer
	std::vector<std::pair< SpotLight*,Texture*>> spotLight;
public:
	GfxCmd_SetLightBuffer();
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
