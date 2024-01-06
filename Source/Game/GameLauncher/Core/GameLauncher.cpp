
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h> 

#include "GameLauncher.h"

#include <assert.h> 
#include <fstream>
#include <memory> 

#include <Tools/Logging/Logging.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/ThirdParty/nlohmann/json.hpp>   
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>


using json = nlohmann::json;

void GameLauncher::Init()
{
	GLLogger.Log("GameLauncher Init");
}

void GameLauncher::Start()
{

	GameObjectManager& gom = GameObjectManager::Get();

#pragma region BaseSetup
	myCustomHandler = gom.CreateGameObject();
	myMesh = gom.CreateGameObject();


	{
		GameObject camera = gom.CreateGameObject();
		camera.AddComponent<cCamera>();
		gom.SetLastGOAsCamera();


		auto& transform = camera.AddComponent<Transform>();
		transform.SetPosition(0,25,0);
		transform.SetRotation(90,0,0);

	}
	{
		GameObject worldRoot = gom.CreateGameObject();
		gom.SetLastGOAsWorld();

		worldRoot.AddComponent<FrameStatistics>();
		worldRoot.AddComponent<RenderMode>();
		worldRoot.AddComponent<Skybox>();
		worldRoot.AddComponent<cLight>(eLightType::Directional);
		Transform& transform = worldRoot.AddComponent<Transform>();
		transform.SetRotation(0,45,-45);
		cLight& pLight = worldRoot.GetComponent<cLight>();
		pLight.SetColor(Vector3f(1,1,1));
		pLight.SetPower(1.0f);
		pLight.BindDirectionToTransform(true);

		if(gom.GetAllComponents<BackgroundColor>().empty())
		{
			worldRoot.AddComponent<BackgroundColor>(Vector4f(1.0f,1.0f,1.0f,1.0f));
		}
	}

	{
		GameObject test3 = gom.CreateGameObject();
		test3.AddComponent<cMeshRenderer>("Models/SteelFloor.fbx");
		//test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");
		auto& transform = test3.AddComponent<Transform>();
		transform.SetPosition(0,-1.5f,0);
		transform.SetScale(Vector3f(50.0f,1.0f,50.0f));
		transform.SetGizmo(false);
	}

#if WorkingOnPngLoading
	{
		GameObject sponza = gom.CreateGameObject();
		sponza.AddComponent<cMeshRenderer>("Models/Sponza.fbx");
		//test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");
		auto& transform = sponza.AddComponent<Transform>();
		transform.SetPosition(50,0,0);
		transform.SetGizmo(false);
	}
#endif

#pragma endregion



	//Decisiontree 1
	int actorAmount = 2;
	int wellAmount = 1;

#pragma region Collider
	std::vector<GameObject> colliders;
	//colliders.resize(collidersAmount);
	auto colliderPositions =
	{
		Vector3<Vector3f>(25.5f * Vector3f(1,0,0), Vector3f(0,0,0),Vector3f(1,1,25)),
		Vector3<Vector3f>(25.5f * Vector3f(-1,0,0),Vector3f(0,0,0),Vector3f(1,1,25)),
		Vector3<Vector3f>(25.5f * Vector3f(0,0,1), Vector3f(0,0,0),Vector3f(25,1,1)),
		Vector3<Vector3f>(25.5f * Vector3f(0,0,-1),Vector3f(0,0,0),Vector3f(25,1,1)),
		Vector3<Vector3f>(20.f * Vector3f(0,0,1),Vector3f(0,0,0),Vector3f(1,1,10)),
		Vector3<Vector3f>(20.f * Vector3f(0,0,-1),Vector3f(0,0,0),Vector3f(1,1,10)),
	};

	for(const auto& transformData : colliderPositions)
	{
		//Drone
		auto object = gom.CreateGameObject();
		auto& transform = object.AddComponent<Transform>();

		transform.SetPosition(transformData[0]);
		transform.SetRotation(transformData[1]);
		transform.SetScale(transformData[2]);

		auto& mesh = object.AddComponent<cMeshRenderer>("Models/Cube.fbx");
		mesh.SetMaterialPath("Materials/C64Separatist.json");
		object.AddComponent<cCollider>();
		colliders.push_back(object);
	}
#pragma endregion


#pragma region Entities
	std::vector<GameObject> entities;
	entities.resize(actorAmount);
	for(auto& obj : entities)
	{
		//Drone
		obj = gom.CreateGameObject();
		auto& transform = obj.AddComponent<Transform>();
		transform.SetPosition(RandomEngine::RandomInRange<float>(-20,20),0,RandomEngine::RandomInRange<float>(-20,20));
		transform.SetRotation(0,RandomEngine::RandomInRange(0.f,360.f),0);

		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/C64Seeker.fbx");
		mesh.SetMaterialPath("Materials/C64Seeker.json");
		obj.AddComponent<cCollider>();
		obj.SetLayer(Layer::Entities);
	}
#pragma endregion


#pragma region Healtwell
	std::vector<GameObject> well;
	well.resize(wellAmount); //Safety resize if we dont add more it wont realloc and span wont loose connection  
	for(auto& obj : well)
	{
		float x = RandomEngine::RandomInRange(-20.0f,20.0f);
		float z = RandomEngine::RandomInRange(-20.0f,20.0f);

		//Drone
		obj = gom.CreateGameObject();
		auto& transform = obj.AddComponent<Transform>();
		transform.SetPosition(x,0,z);
		transform.SetRotation(90,0,0);

		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/Well.fbx");
		mesh.SetMaterialPath("Materials/C64Player.json");
	}
#pragma endregion

	auto colliderPollingStation = std::make_shared<MultipleTargets_PollingStation>(colliders);
	auto formationPollingStation = std::make_shared<MultipleTargets_PollingStation>(entities);
	auto wellPollingStation = std::make_shared<MultipleTargets_PollingStation>(well);

	AIPollingManager::Get().AddStation("Healing",wellPollingStation);
	AIPollingManager::Get().AddStation("Colliders",colliderPollingStation);
	AIPollingManager::Get().AddStation("Targets",formationPollingStation);


	{
		/*auto& actor = entities[0].AddComponent<cActor>();
		actor.SetController(new AIController(
			AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Colliders").get(),
			AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Targets").get())
		);*/


		auto& actor = entities[0].AddComponent<cActor>();
		actor.SetController(new DecisionTreeController());
	}

	{
		auto& actor = entities[1].AddComponent<cActor>();
		actor.SetController(new DecisionTreeController());
	}

	GLLogger.Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
	delta;
	OPTICK_EVENT();

	//Movement1
	AIEventManager::Instance().Update();
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::K))
	{
		GraphicsEngine::Get().GetSettings().DebugRenderer_Active = !GraphicsEngine::Get().GetSettings().DebugRenderer_Active;
	}

	//Other
	{
		for(auto& i : GameObjectManager::Get().GetAllComponents<cLight>()) // TODO optimize this once you have a lot of lights and lightobjects
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	}

	//Transform& pLight = GameObjectManager::GetInstance().GetWorldRoot().GetComponent<Transform>();
	//pLight.Rotate(0,delta,0);
}
