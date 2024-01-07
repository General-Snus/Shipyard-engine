#include "AssetManager.pch.h"
#include "../Skybox.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>

Skybox::Skybox(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myCubeMap = GraphicsEngine::Get().GetDefaultTexture(eTextureType::CubeMap);
}

Skybox::Skybox(const unsigned int anOwnerId,const std::filesystem::path aPath) : Component(anOwnerId)
{
	AssetManager::Get().LoadAsset<TextureHolder>(aPath,myCubeMap);
	myCubeMap->SetTextureType(eTextureType::CubeMap);
}

void Skybox::Update()
{
}

void Skybox::Render()
{
	OPTICK_EVENT();
	if(!myCubeMap->GetRawTexture()->IsValid())
	{
		return;
	}
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderSkybox>(myCubeMap->GetRawTexture());
}
