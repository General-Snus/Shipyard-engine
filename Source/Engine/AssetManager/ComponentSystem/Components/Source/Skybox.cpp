#include "Engine/AssetManager/AssetManager.pch.h"

#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include "../Skybox.h"

#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Tools/ImGui/ImGui/imgui.h"

Skybox::Skybox(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{
	myCubeMap = GraphicsEngine::Get().GetDefaultTexture(eTextureType::CubeMap);
}

Skybox::Skybox(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aPath) : Component(anOwnerId,aManager)
{
	AssetManager::Get().ForceLoadAsset<TextureHolder>(aPath,myCubeMap);
	AssetManager::Get().ForceLoadAsset<Material>(aPath,mySkyboxMaterial);

	myCubeMap->SetTextureType(eTextureType::CubeMap);

	if (!myCubeMap->GetRawTexture())
	{
		return;
	}
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderSkybox>(myCubeMap->GetRawTexture());
}

void Skybox::Update()
{
}


bool Skybox::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}

	Reflect<Skybox>();
		return true;
}

void Skybox::Render()
{
	OPTICK_EVENT();
	//RenderData data;

	//data.myMesh = mySkyboxSphere;
	//data.overrideMaterial.push_back(mySkyboxMaterial);

	//GraphicsEngine::Get().DeferredCommand<GfxCmd_SetDepthState>(GraphicsEngine::eDepthStencilStates::DSS_ReadWrite);
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_RenderMesh>(data,Matrix::CreateScaleMatrix(Vector3f(m_Radius)),false);
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_SetDepthState>(DepthState::DS_Default);
	//reset depth state

}
