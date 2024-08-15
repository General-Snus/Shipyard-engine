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

// Task 7 AI
#include <Engine/AssetManager/Objects/AI/AgentSystem/AIEventManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/PollingStations/Target_PollingStation.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/StateMachine/StateMachineBase.h>

#include <Engine/AssetManager/Objects/AI/AgentSystem/BehaviourTree/BrainTree.h>
#include <Engine/PersistentSystems/ArtificialInteligence/AICommands/AICommands.h>
#include <Game/GameLauncher/TaskSpecificImplementation/BehaviourTree/BehaviorTreeController.h>
#include <Game/GameLauncher/TaskSpecificImplementation/DecicionTree/DecisionTreeController.h>
#include <Game/GameLauncher/TaskSpecificImplementation/StateMachine/StateMachineController.h>

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
        transform.SetPosition(0, 30, 0);
        transform.SetRotation(90, 0, 0);

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

    Transform &pLight = Scene::ActiveManager().GetWorldRoot().GetComponent<Transform>();
    constexpr float rotSpeed = 25.f;
    if (Input::IsKeyHeld(static_cast<int>(Keys::NUMPAD6)))
    {
        pLight.Rotate(rotSpeed * delta, 0, 0);
    }
    if (Input::IsKeyHeld(static_cast<int>(Keys::NUMPAD3)))
    {
        pLight.Rotate(-rotSpeed * delta, 0, 0);
    }
}

void GameLauncher::LocalFunction()
{
    Logger::Log("Local Function Called");
}
