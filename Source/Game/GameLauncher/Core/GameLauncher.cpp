#include "ShipyardEngine.pch.h"

#include "GameLauncher.h"

#pragma optimize("", off)

extern "C"
{
    inline GAME_API GameLauncher *EntrypointMain()
    {
        return new YourGameLauncher();
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

            auto &cameraComponent = camera.AddComponent<Camera>();
            cameraComponent.SetActive(true);

            cameraComponent.transform().SetPosition(0, 30, -30);
            camera.transform().SetRotation(30, 0, 0);
        }

        {
            player = GameObject::Create("Player");
            Scene::ActiveManager().SetLastGOAsPlayer();

            player.AddComponent<MeshRenderer>("Models/C64.fbx");
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

            worldRoot.SetName("Directional Light");

            auto &light = worldRoot.AddComponent<Light>(eLightType::Directional);
            light.SetColor("White");
            light.SetPower(2.0f);
            light.BindDirectionToTransform(true);
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
    AIEventManager::Instance().Update();
}

void YourGameLauncher::SyncServices(ServiceLocator &serviceLocator)
{
    ServiceLocator::SyncInstances(serviceLocator);
    ImGui::InitializeOnNewContext(serviceLocator.GetService<ImGui::ImGuiContextHolder>());
};

extern "C" BOOL WINAPI DllMain(HINSTANCE const instance, // handle to DLL module
                               DWORD const reason,       // reason for calling function
                               LPVOID const reserved)    // reserved
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
