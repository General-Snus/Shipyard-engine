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
	if (GetRenderer().GetAmountOfRenderJob())
	{
		return false;
	}

	renderTarget->isBeingLoaded = true;
	const auto res = Vector2ui(512, 512);
	{
		auto& camera = GetRenderer().newScene->GetGOM().GetCamera().GetComponent<Camera>();
		camera.SetResolution(res);
		auto& transform = camera.transform();
		const Vector3f position = meshAsset->Bounds.GetCenter() + 0.5f * Vector3f(-meshAsset->Bounds.GetRadius(), meshAsset->Bounds.GetRadius(), -meshAsset->Bounds.GetRadius());
		transform.SetPosition(position);
		transform.LookAt(meshAsset->Bounds.GetCenter());

		transform.Update();
		camera.Update();
	}

	{
		const auto target = GetRenderer().newScene->GetGOM().GetPlayer();
		auto& mr = target.GetComponent<MeshRenderer>();
		mr.SetNewMesh(meshAsset);
		mr.SetMaterial(material);
	}

	const auto newViewport = std::make_shared<Viewport>(true, res, GetRenderer().newScene, renderTarget);
	GetRenderer().AddRenderJob(newViewport);
	return true;
}
