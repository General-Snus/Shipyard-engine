
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
	GameObjectManager& gom = GameObjectManager::GetInstance();

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
		test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");
		auto& transform = test3.AddComponent<Transform>();
		transform.SetPosition(0,-1.5f,0);
		transform.SetScale(Vector3f(50.0f,1.0f,50.0f));
		transform.SetGizmo(false);
	}
#pragma endregion

	//Movement1 
	SY::UUID player;
	std::vector<GameObject> entities;
	entities.resize(15); //Safety resize if we dont add more it wont realloc and span wont loose connection

	auto wanderer = std::span(entities.begin(),1);//1 from position 0
	auto seekers = std::span(entities.begin() + 1,4); //4 from position 1
	auto separatists = std::span(entities.begin() + 5,10); //10 from position 6

	for(auto& obj : entities)
	{
		float x = RandomEngine::RandomInRange<float>(-10.f,10.f);
		float z = RandomEngine::RandomInRange<float>(-10.f,10.f);

		//Drone
		GameObject drone = gom.CreateGameObject();
		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(x,0,z);

		obj = drone;
	}

	for(auto& obj : wanderer)
	{
		//Drone 
		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Player.json"); 

		auto& actor = obj.AddComponent<cActor>();
		actor.SetController(new WanderController(obj));
	}
	
	auto* seekerStation = new Target_PollingStation(wanderer[0]);
	for(auto& obj : seekers)
	{ 
		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Enemy.json");

		auto& actor = obj.AddComponent<cActor>();
		actor.SetController(new SeekerController(seekerStation,obj));
	}

	auto* separatistsStation = new MultipleTargets_PollingStation(entities);
	for(auto& obj : separatists)
	{
		auto& mesh = obj.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Enemy.json");

		auto& actor = obj.AddComponent<cActor>();
		actor.SetController(new SeparationController(separatistsStation,obj)); // how do i remove the object itself
	} 

	GLLogger.Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
	OPTICK_EVENT()
		//Movement1
		AIEventManager::Instance().Update();

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::NUMPAD8))
	{
		myCustomHandler.GetComponent<Transform>().Move(0,0,1 * delta);
	}
	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::NUMPAD2))
	{
		myCustomHandler.GetComponent<Transform>().Move(0,0,-1 * delta);
	}
	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::NUMPAD6))
	{
		myCustomHandler.GetComponent<Transform>().Move(1 * delta,0,0);
	}
	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::NUMPAD4))
	{
		myCustomHandler.GetComponent<Transform>().Move(-1 * delta,0,0);
	}


	//Other
	{
		for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>()) // TODO optimize this once you have a lot of lights and lightobjects
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	}

	//Transform& pLight = GameObjectManager::GetInstance().GetWorldRoot().GetComponent<Transform>();
	//pLight.Rotate(0,delta,0);
}
