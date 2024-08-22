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

// Root is returned
Transform &CreateRoom(Vector3f rootPosition, Vector3f Boundries)
{
    GameObject root = GameObject::Create("RoomRoot");

    const std::vector<Vector3f> positionList = {
        {0, 0, 0},
        {Boundries.x / 2, Boundries.y / 2, 0},
        {-Boundries.x / 2, Boundries.y / 2, 0},
        {0, Boundries.y / 2, Boundries.z / 2},
    };

    constexpr float thickness = 0.1f;
    const std::vector<Vector3f> scaleList = {
        {Boundries.x, thickness, Boundries.z},
        {thickness, Boundries.y, Boundries.z},
        {thickness, Boundries.y, Boundries.z},
        {Boundries.x, Boundries.y, thickness},
    };

    for (auto [scale, position] : std::views::zip(scaleList, positionList))
    {
        GameObject wall = GameObject::Create("Wall");

        wall.transform().SetPosition(position);
        wall.transform().SetScale(scale);
        wall.transform().SetParent(root.transform());

        wall.AddComponent<cMeshRenderer>("Models/Cube.fbx");
        wall.AddComponent<cCollider>();
    }

    GameObject pointLight = GameObject::Create("light");
    pointLight.transform().SetPosition(0, Boundries.y / 2, 0);
    pointLight.transform().SetParent(root.transform());

    auto &light = pointLight.AddComponent<cLight>(eLightType::Point);
    light.SetRange(Boundries.y * 2);
    light.SetPower(1000);

    root.transform().SetPosition(rootPosition);
    return root.transform();
}

void GameLauncher::Start()
{
    OPTICK_EVENT();
    m_CustomKeyCallback.AddListener(std::bind(&GameLauncher::LocalFunction, this));
    {
    }

    // Ett rumm för varje uppgift
    for (int task = 0; task < 4; task++)
    {
        CreateRoom({task * 40.0f, -1.f, 0.f}, {40, 40, 40});
    }
    Object1_Room1 = GameObject::Create("Object1_Room1");
    Object1_Room1.AddComponent<cMeshRenderer>("Models/Cube.fbx");
    Object1_Room1.AddComponent<GraphComponent>("Graphs/Main1Graph.Graph");
    Object1_Room1.AddComponent<cCollider>();
    Object1_Room1.transform().SetPosition(-15, 4, 0);
    Object1_Room1.transform().SetScale(5, 5, 5);

    Object2_Room1 = GameObject::Create("Object2_Room1");
    Object2_Room1.AddComponent<cMeshRenderer>("Models/Cube.fbx");
    Object2_Room1.AddComponent<GraphComponent>("Graphs/Main1Graph.Graph");
    Object2_Room1.AddComponent<cCollider>();
    Object2_Room1.transform().SetPosition(0, 4, 15);
    Object2_Room1.transform().SetScale(5, 5, 5);

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

    constexpr float area = 7.5f;

    static float direction = 1;
    if (Object1_Room1.transform().GetPosition().x > area)
    {
        direction = -1;
    }
    if (Object1_Room1.transform().GetPosition().x < -area)
    {
        direction = 1;
    }

    Object1_Room1.transform().Move(5 * direction * Timer::GetDeltaTime(), 0, 0);

    static float direction2 = 1;
    if (Object2_Room1.transform().GetPosition().z > area)
    {
        direction2 = -1;
    }
    if (Object2_Room1.transform().GetPosition().z < -area)
    {
        direction2 = 1;
    }

    Object2_Room1.transform().Move(0, 0, 5 * direction2 * Timer::GetDeltaTime());
}

void GameLauncher::LocalFunction()
{
    Logger::Log("Local Function Called");
}
