#include "GraphicsEngine.pch.h"

#include "GraphicsEngineUtilities.h"
#include <Editor/Editor/Windows/EditorWindows/Viewport.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/PersistentSystems/Scene.h>

bool GraphicsEngineUtilities::GenerateSceneForIcon(std::shared_ptr<Mesh> meshAsset,
												   std::shared_ptr<TextureHolder> renderTarget,
												   std::shared_ptr<Material> material)
{
	OPTICK_EVENT();
	if (GraphicsEngineInstance.GetAmountOfRenderJob())
	{
		return false;
	}

	renderTarget->isBeingLoaded = true;
	{
		auto &transform = GraphicsEngineInstance.newScene->GetGOM().GetCamera().GetComponent<Transform>();
		const Vector3f position = meshAsset->Bounds.GetCenter() + Vector3f(0, 0, -meshAsset->Bounds.GetRadius());
		transform.SetPosition(position);
		transform.Update();
	}

	{
		auto target = GraphicsEngineInstance.newScene->GetGOM().GetPlayer();
		auto &mr = target.GetComponent<MeshRenderer>();
		mr.SetNewMesh(meshAsset);
		mr.SetMaterial(material);
	}

	const auto res = Vector2f(1920.f, 1080.f);
	auto newViewport = std::make_shared<Viewport>(true, res, GraphicsEngineInstance.newScene, renderTarget);
	GraphicsEngineInstance.AddRenderJob(newViewport);
	return true;
}
