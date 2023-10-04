#include "AssetManager.pch.h"
#include "Skybox.h"

Skybox::Skybox(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myCubeMap = GraphicsEngine::Get().GetDefaultTexture(eTextureType::CubeMap);
}

Skybox::Skybox(const unsigned int anOwnerId,const std::filesystem::path aPath) : Component(anOwnerId)
{
	AssetManager::GetInstance().LoadAsset<TextureHolder>(aPath,myCubeMap);
	myCubeMap->SetTextureType(eTextureType::CubeMap);
}

void Skybox::Update()
{
}

void Skybox::Render()
{
	if(!myCubeMap->GetRawTexture()->IsValid())
	{
		return;
	}
	GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderSkybox>(myCubeMap->GetRawTexture());
}
