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
        GameObject camera = GameObject::Create();
        camera.SetName("Camera");
        camera.AddComponent<cMeshRenderer>("Models/Camera/Camera.fbx");
        auto &cameraComponent = camera.AddComponent<cCamera>();
        Scene::ActiveManager().SetLastGOAsCamera();
        cameraComponent.SetActive(true);
        auto &transform = camera.AddComponent<Transform>();
        transform.SetPosition(0, 30, 0);
        transform.SetRotation(90, 0, 0);
    }
}

void GameLauncher::Start()
{
    OPTICK_EVENT();
    m_CustomKeyCallback.AddListener(std::bind(&GameLauncher::LocalFunction, this));
#pragma region BaseSetup
    {
        GameObject worldRoot = GameObject::Create();
        Scene::ActiveManager().SetLastGOAsWorld();
        worldRoot.SetName("WordRoot");
        // worldRoot.AddComponent<Skybox>();

        auto &pLight = worldRoot.AddComponent<cLight>(eLightType::Directional);
        pLight.SetColor("White");
        pLight.SetPower(2.0f);
        pLight.BindDirectionToTransform(true);
    }

    // Decisiontree 1
    int actorAmount = 2;
    int wellAmount = 2;

#pragma region Collider
    std::vector<GameObject> colliders;
    // colliders.resize(collidersAmount);
    auto colliderPositions = {
        Vector3<Vector3f>(25.5f * Vector3f(1, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 25)),
        Vector3<Vector3f>(25.5f * Vector3f(-1, 0, 0), Vector3f(0, 0, 0), Vector3f(1, 1, 25)),
        Vector3<Vector3f>(25.5f * Vector3f(0, 0, 1), Vector3f(0, 0, 0), Vector3f(25, 1, 1)),
        Vector3<Vector3f>(25.5f * Vector3f(0, 0, -1), Vector3f(0, 0, 0), Vector3f(25, 1, 1)),
        Vector3<Vector3f>(20.f * Vector3f(0, 0, 1), Vector3f(0, 0, 0), Vector3f(1, 1, 10)),
        Vector3<Vector3f>(20.f * Vector3f(0, 0, -1), Vector3f(0, 0, 0), Vector3f(1, 1, 10)),
    };

    for (const auto &[number, transformData] : colliderPositions | std::ranges::views::enumerate)
    {
        // Drone
        auto object = GameObject::Create(std::format("Wall_{}", number));

        object.transform().SetPosition(transformData[0]);
        object.transform().SetRotation(transformData[1]);
        object.transform().SetScale(transformData[2]);

        auto &mesh = object.AddComponent<cMeshRenderer>("Models/Cube.fbx");
        mesh.SetMaterialPath("Materials/C64.json");
        object.AddComponent<cCollider>();
        colliders.push_back(object);
    }
#pragma endregion

#pragma region Entities
    std::vector<GameObject> entities;
    entities.resize(actorAmount);
    int index = 0;
    for (auto &obj : entities)
    {
        // Drone
        obj = GameObject::Create(std::format("Entitity_{}", index));
        auto &transform = obj.AddComponent<Transform>();
        transform.SetPosition(RandomEngine::RandomInRange<float>(-20, 20), 0,
                              RandomEngine::RandomInRange<float>(-20, 20));
        transform.SetRotation(0, RandomEngine::RandomInRange(0.f, 360.f), 0);
        obj.AddComponent<cCollider>();
        obj.SetLayer(Layer::Entities);
        index++;
    }
#pragma endregion

#pragma region Healtwell
    std::vector<GameObject> well;
    well.resize(wellAmount); // Safety resize if we dont add more it wont realloc and span wont loose connection
    index = 0;
    for (auto &obj : well)
    {
        float x = RandomEngine::RandomInRange(-20.0f, 20.0f);
        float z = RandomEngine::RandomInRange(-20.0f, 20.0f);

        // Drone
        obj = GameObject::Create(std::format("Well_{}", index));
        obj.transform().SetPosition(x, 0, z);
        obj.transform().SetRotation(90, 0, 0);

        auto &mesh = obj.AddComponent<cMeshRenderer>("Models/Well.fbx");
        mesh.SetMaterialPath("Materials/C64Well.json");
        index++;
    }
#pragma endregion

    auto colliderPollingStation = std::make_shared<MultipleTargets_PollingStation>(colliders);
    auto formationPollingStation = std::make_shared<MultipleTargets_PollingStation>(entities);
    auto wellPollingStation = std::make_shared<MultipleTargets_PollingStation>(well);

    AIPollingManager::Get().AddStation("Healing", wellPollingStation);
    AIPollingManager::Get().AddStation("Colliders", colliderPollingStation);
    AIPollingManager::Get().AddStation("Targets", formationPollingStation);

    //{ // DecisionTree
    //    auto &actor = entities[0].AddComponent<cActor>();
    //    entities[0].SetName("DecisionTreeActor");
    //    actor.SetController(new DecisionTreeController());
    //    auto &mesh = entities[0].AddComponent<cMeshRenderer>("Models/C64Wanderer.fbx");
    //    mesh.SetMaterialPath("Materials/C64Wanderer.json");
    //}

    { // StateMachine
        auto &actor = entities[0].AddComponent<cActor>();
        entities[0].SetName("StateMachineActor");
        actor.SetController(new StateMachineController());
        auto &mesh = entities[0].AddComponent<cMeshRenderer>("Models/C64Seeker.fbx");
        mesh.SetMaterialPath("Materials/C64Seeker.json");
    }

    { // BehaviourTree
        auto &actor = entities[1].AddComponent<cActor>();
        entities[1].SetName("BehaviourTreeActor");

        // clang-format off
		using namespace BrainTree; 
		BehaviorTree 	tree =  Builder()
			.composite<Selector>("MainSelector")
				.decorator<BehaviourTreeAICommands::IsDead>() 
					.leaf<BehaviourTreeAICommands::DeathSpin>()
				.end()
				.decorator<BehaviourTreeAICommands::IsHealthy>()
				    .composite<Selector>("BattleSelector") 
					    .composite<Sequence>("BattleSequencer")
					        .leaf<BehaviourTreeAICommands::IsTargetAlive>()
					        .leaf<BehaviourTreeAICommands::IsTargetInRange>()
                                .composite<Sequence>("CanFireSequencer") 
						            .leaf<BehaviourTreeAICommands::AlignToTarget>()
						            .leaf<BehaviourTreeAICommands::IsTargetInSight>()
					            .end()
					        .leaf<BehaviourTreeAICommands::ShootAtTarget>()
                        .end()
					    .composite<Sequence>("CanMoveFreelySequence")
					        .decorator<BehaviourTreeAICommands::IsFullyHealed>()
						        .leaf<BehaviourTreeAICommands::MoveFreely>()
				            .end()
					    .end()
					.end()
				.end()
				.leaf<BehaviourTreeAICommands::Retreat>()
			.end()
			.build();
        // clang-format on

        actor.SetController(new BehaviorTreeController(tree));
        auto &mesh = entities[1].AddComponent<cMeshRenderer>("Models/C64.fbx");
        mesh.SetMaterialPath("Materials/C64Separatist.json");
    }
    Logger::Log("GameLauncher start");

#pragma endregion
}

void GameLauncher::Update(float delta)
{
    OPTICK_EVENT();

    if (Input::IsKeyPressed(static_cast<unsigned>(Keys::I)))
    {
        Logger::Log(std::to_string(1.f / delta));
    }
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
