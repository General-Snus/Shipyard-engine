#include "ShipyardEngine.pch.h"

#include "GameLauncher.h"
#include <UserComponent.h>

extern "C"
{
	inline GAME_API GameLauncher *EntrypointMain()
	{
		return new YourGameLauncher();
	}
}
extern "C"
{
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
			GameObject camera = GameObject::Create("Test Camera");
			Scene::ActiveManager().SetLastGOAsCamera();

			camera.AddComponent<MeshRenderer>("Models/Camera/Camera.fbx");

			camera.AddComponent<Camera>();
			camera.transform().SetPosition(0, 5, 0);
			camera.transform().SetRotation(90, 0, 0);
		}

		{
			player = GameObject::Create("Player");
			Scene::ActiveManager().SetLastGOAsPlayer();
			player.AddComponent<MeshRenderer>("Models/C64.fbx");
			player.AddComponent<UserComponent>();
			Scene::ActiveManager().GetCamera().transform().SetParent(player.transform());
		}

		{
			GameObject SkySphere = GameObject::Create("SkySphere");
			auto &mesh = SkySphere.AddComponent<MeshRenderer>("Materials/MaterialPreviewMesh.fbx");
			mesh.SetMaterialPath("Materials/SkySphere.json");

			SkySphere.transform().SetScale(-100000, -100000, -100000);
		}

		{
			GameObject worldRoot = GameObject::Create();
			Scene::ActiveManager().SetLastGOAsWorld();

			worldRoot.SetName("SkyLight");

			auto &light = worldRoot.AddComponent<Light>(eLightType::Directional);
			light.SetColor("White");
			light.SetPower(2.0f);
			light.BindDirectionToTransform(true);
		}

		{
			GameObject sponza = GameObject::Create("sponza");
			sponza.AddComponent<MeshRenderer>("Models/Sponza/Sponza.fbx");

			sponza.transform().SetScale(.01f);
		}
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
}

void YourGameLauncher::SyncServices(ServiceLocator &serviceLocator)
{
	ServiceLocator::SyncInstances(serviceLocator);
	ImGui::InitializeOnNewContext(serviceLocator.GetService<ImGui::ImGuiContextHolder>());
};

extern "C" BOOL WINAPI DllMain(HINSTANCE const instance, // handle to DLL module
							   DWORD const reason,		 // reason for calling function
							   LPVOID const reserved)	 // reserved
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
