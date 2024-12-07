#include "ShipyardEngine.pch.h"

#include "GameLauncher.h"

#include <UserComponent.h>
#include "Editor/Editor/Windows/EditorWindows/Viewport.h"
#include "Engine/PersistentSystems/Physics/Raycast.h"
#include "PillarGame/PillarGameComponents.h"

extern "C" {
inline GAME_API GameLauncher* EntrypointMain()
{
	return new YourGameLauncher();
}
}

extern "C" {
inline GAME_API void ExitPoint(HMODULE handle)
{
	FreeLibraryAndExitThread(handle, 0);
}
}

void YourGameLauncher::Init()
{
	OPTICK_EVENT();
	{
		{
			GameObject SkySphere = GameObject::Create("SkySphere");
			auto&      mesh = SkySphere.AddComponent<MeshRenderer>("Materials/MaterialPreviewMesh.fbx");
			mesh.SetMaterialPath("Materials/SkySphere.json");

			SkySphere.transform().SetScale(-100000, -100000, -100000);
		}

		{
			GameObject worldRoot = GameObject::Create();
			Scene::ActiveManager().SetLastGOAsWorld();

			worldRoot.SetName("SkyLight");
			worldRoot.transform().SetRotation(45, 45, 0);
			auto& light = worldRoot.AddComponent<Light>(eLightType::Directional);
			light.SetIsShadowCaster(true);
			light.SetColor("White");
			light.SetPower(2.0f);
			light.BindDirectionToTransform(true);
		}

		constexpr float pillarRadius = 1.0f;
		auto            groundPos = Vector3f(0, -.5f, 0);
		SpawnGround(groundPos);
		SpawnPillar(groundPos);
		SpawnHooks(50, pillarRadius, groundPos);
		SpawnPlayer(0, pillarRadius, {groundPos.x, -0.35f, groundPos.z});
	}
}

void YourGameLauncher::Start()
{
	OPTICK_EVENT();
	Logger.Log("GameLauncher start");
}

void YourGameLauncher::Update(float delta)
{
	OPTICK_EVENT();
	UNREFERENCED_PARAMETER(delta);
	auto& manager = Scene::ActiveManager();
	for (auto& element : manager.GetAllComponents<PlayerComponent>())
	{
		if (Input.IsKeyHeld(Keys::RIGHT))
		{
			element.transform().Rotate(0, -50.0f * delta, 0);
		}

		if (Input.IsKeyHeld(Keys::LEFT))
		{
			element.transform().Rotate(0, 50.0f * delta, 0);
		}

		if (Input.IsKeyPressed(Keys::SPACE))
		{
			Physics::RaycastHit hit;
			const auto&         cameraTransform = manager.GetCamera().transform();
			const auto&         camera = manager.GetCamera().GetComponent<Camera>();

			const auto coord = EditorInstance.GetMainViewport()->getCursorInWindowPostion();
			if (Raycast(cameraTransform.GetPosition(), camera.GetPointerDirection(coord),
			            hit))
			{
				Logger.Log("You hit something " + hit.objectHit.GetName(), true);
			}
		}
	}
}

void YourGameLauncher::SyncServices(ServiceLocator& serviceLocator)
{
	ServiceLocator::SyncInstances(serviceLocator);
	InitializeOnNewContext(serviceLocator.GetService<ImGui::ImGuiContextHolder>());
};

extern "C" BOOL WINAPI DllMain(const HINSTANCE instance, // handle to DLL module
                               const DWORD     reason,   // reason for calling function
                               const LPVOID    reserved) // reserved
{
	instance;
	reserved;
	// Perform actions based on the reason for calling.
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
