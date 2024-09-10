#include <Engine/AssetManager/AssetManager.pch.h>

#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "GameLauncher.h"
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Collider.h>
#include <Engine/AssetManager/ComponentSystem/Components/DEBUGCOMPONENTS/FrameStatistics.h>
#include <Engine/AssetManager/ComponentSystem/Components/DEBUGCOMPONENTS/RenderMode.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXDynamicBody.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXStaticBody.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>

#include <Engine/AssetManager/ComponentSystem/Components/GraphComponent.h>
#include <Engine/AssetManager/Objects/BaseAssets/GraphAsset.h>
#include <Tools/Utilities/DataStructures/Ranges.hpp>

void GameLauncher::Init()
{
    OPTICK_EVENT();
    {
        GameObject camera = GameObject::Create("Main Camera");
        camera.AddComponent<cMeshRenderer>("Models/Camera/Camera.fbx");
        auto &cameraComponent = camera.AddComponent<cCamera>();
        Scene::ActiveManager().SetLastGOAsCamera();
        cameraComponent.SetActive(true);
        auto &transform = camera.AddComponent<Transform>();
        transform.SetPosition(0, 30, -30);
        transform.SetRotation(30, 0, 0);

        GameObject SkySphere = GameObject::Create("SkySphere");
        auto &mesh = SkySphere.AddComponent<cMeshRenderer>("Materials/MaterialPreviewMesh.fbx");
        mesh.SetMaterialPath("Materials/SkySphere.json");
        SkySphere.transform().SetScale(-100000, -100000, -100000);

        GameObject worldRoot = GameObject::Create();
        Scene::ActiveManager().SetLastGOAsWorld();
        worldRoot.SetName("Directional Light");

        auto &pLight = worldRoot.AddComponent<cLight>(eLightType::Directional);
        pLight.SetColor("White");
        pLight.SetPower(2.0f);
        pLight.BindDirectionToTransform(true);
    }
}

void GameLauncher::Start()
{
    OPTICK_EVENT();
    m_CustomKeyCallback.AddListener(std::bind(&GameLauncher::LocalFunction, this));
    {
    }

    Logger::Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
    OPTICK_EVENT();
    delta;

    AIEventManager::Instance().Update();
    if (Input::IsKeyPressed(static_cast<int>(Keys::K)))
    {
        GraphicsEngine::Get().GetSettings().DebugRenderer_Active =
            !GraphicsEngine::Get().GetSettings().DebugRenderer_Active;
    }

    if (Input::IsKeyPressed(static_cast<int>(Keys::F5)))
    {
        ApplicationState::filter = DebugFilter::NoFilter;
    }

    if (Input::IsKeyPressed(static_cast<int>(Keys::F6)))
    {
        ApplicationState::filter = static_cast<DebugFilter>((static_cast<int>(ApplicationState::filter) + 1) %
                                                            static_cast<int>(DebugFilter::count));
    }

    if (Input::IsKeyPressed(Keys::P))
    {
        m_CustomKeyCallback.Invoke();
    }
}

void GameLauncher::LocalFunction()
{
    Logger::Log("Local Function Called");
}
