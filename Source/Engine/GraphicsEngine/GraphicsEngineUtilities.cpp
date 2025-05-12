#include "GraphicsEngine.pch.h"

#include "GraphicsEngineUtilities.h"
#include <Editor/Editor/Windows/EditorWindows/Viewport.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/PersistentSystems/Scene.h>
#include "Renderer.h"

bool GraphicsEngineUtilities::GenerateSceneForIcon(std::shared_ptr<Mesh> meshAsset,
												   std::shared_ptr<TextureHolder> renderTarget,
												   std::shared_ptr<Material> material)
{
	OPTICK_EVENT();
	if (RENDERER.GetAmountOfRenderJob())
	{
		return false;
	}

	renderTarget->isBeingLoaded = true;
	const auto res = Vector2ui(1920, 1080);
	{
		auto& camera = RENDERER.newScene->GetGOM().GetCamera().GetComponent<Camera>();
		camera.SetResolution(res);
		auto &transform = camera.transform();
		const Vector3f position = meshAsset->Bounds.GetCenter() + Vector3f(0, 0, -meshAsset->Bounds.GetRadius());
		transform.SetPosition(position);
		transform.Update();
	}

	{
		const auto target = RENDERER.newScene->GetGOM().GetPlayer();
		auto &mr = target.GetComponent<MeshRenderer>();
		mr.SetNewMesh(meshAsset);
		mr.SetMaterial(material);
	}

	const auto newViewport = std::make_shared<Viewport>(true, res, RENDERER.newScene, renderTarget);
	RENDERER.AddRenderJob(newViewport);
	return true;
}
