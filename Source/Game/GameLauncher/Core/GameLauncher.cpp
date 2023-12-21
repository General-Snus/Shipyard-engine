
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h> 

#include "GameLauncher.h"

#define WIN32_LEAN_AND_MEAN 
#include "Windows.h"

#include <memory>
#include <string>
#include <stringapiset.h>  
#include <fstream>
#include <streambuf>
#include <assert.h> 

#include <Tools/Utilities/Math.hpp>
#include <Tools/ThirdParty/nlohmann/json.hpp>   
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Logging/Logging.h>
#include <Tools/Optick/src/optick.h>  

//#define ParticleSystemToggle

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

	//Movement1  
	int actorAmount = 2;
	int collidersAmount = 2;


	SY::UUID player;
	std::vector<GameObject> colliders;
	colliders.resize(collidersAmount);
	std::vector<Vector2f> colliderPositions =
	{
		Vector2f(1,0),
		Vector2f(-1,0)
	};
	for(size_t i = 0; i < collidersAmount; i++)
	{
		float position = 15.0f;
		float x = position * colliderPositions[i].x;
		float z = position * colliderPositions[i].y;

		//Drone
		colliders[i] = gom.CreateGameObject();
		auto& transform = colliders[i].AddComponent<Transform>();
		transform.SetPosition(x,0,z);
		transform.SetScale(1.f,1.f,5.f);

		auto& mesh = colliders[i].AddComponent<cMeshRenderer>("Models/Cube.fbx");
		mesh.SetMaterialPath("Materials/C64Separatist.json");

		 colliders[i].AddComponent<cCollider>();  
	}

	std::vector<GameObject> entities;
	entities.resize(actorAmount); //Safety resize if we dont add more it wont realloc and span wont loose connection


	for(auto& obj : entities)
	{
		float x = RandomEngine::RandomInRange<float>(-10.f,10.f);
		float z = RandomEngine::RandomInRange<float>(-10.f,10.f);

		//Drone
		obj = gom.CreateGameObject();
		auto& transform = obj.AddComponent<Transform>();
		transform.SetPosition(x,0,z);
		transform.SetRotation(
			0,
			RandomEngine::RandomInRange(0.f,360.f),
			0);


		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/C64Seeker.fbx");
		mesh.SetMaterialPath("Materials/C64Seeker.json");


		obj.AddComponent<cCollider>();
	}

	auto* colliderPollingStation = new MultipleTargets_PollingStation(colliders);
	auto* formationPollingStation = new MultipleTargets_PollingStation(entities);
	for(auto& obj : entities)
	{
		auto& actor = obj.AddComponent<cActor>();
		actor.SetController(new AIController(colliderPollingStation,formationPollingStation,obj));
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
