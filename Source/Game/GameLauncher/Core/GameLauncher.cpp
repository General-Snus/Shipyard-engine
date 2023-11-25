
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


#include <Objects/AI/AgentSystem/AIEventManager.h>
#include <Objects/AI/AgentSystem/Controllers/EventController.h>
#include <Objects/AI/AgentSystem/PollingStation.h>
#include <Objects/AI/AgentSystem/Controllers/PollingController.h>
#include <Objects/AI/AgentSystem/Actor.h>

#include <Engine/AssetManager/ComponentSystem/Components/ActorSystem/cActor.h>

//#define ParticleSystemToggle

using json = nlohmann::json;

void GameLauncher::Init()
{
	GLLogger.Log("GameLauncher Init");
}

void GameLauncher::Start()
{
	GameObjectManager& gom = GameObjectManager::GetInstance();
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
		transform.SetRotation(0,0,-45);
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
		test3.AddComponent<Transform>();
		test3.GetComponent<Transform>().SetPosition(0,-1.25,0);
	}

	//Movement1


	SY::UUID player;
	std::vector<cActor*> drones;
	drones.reserve(3);
	//World interfacing
	{
		//Drone
		myCustomHandler = gom.CreateGameObject();
		auto& mesh = myCustomHandler.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Player.json");
		auto& transform = myCustomHandler.AddComponent<Transform>();
		transform.SetPosition(0,0,0);

		auto& actor = myCustomHandler.AddComponent<cActor>();
		actor.SetController(new EventController());
		player = myCustomHandler.GetID();
	}

	for(size_t i = 0; i < drones.size(); i++)
	{
		float x = RandomInRange<float>(-10.f,10.f);
		float z = RandomInRange<float>(-10.f,10.f);
		//Drone
		GameObject drone = gom.CreateGameObject();
		auto& mesh = drone.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Enemy.json");
		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(x,0,z);

		auto& actor = drone.AddComponent<cActor>();
		actor.SetController(new EventController());
		AIEventManager::Instance().RegisterListener(eAIEvent::playerHacking,actor.GetController());
	}

	std::vector<SY::UUID> computers;
	computers.resize(3);
	for(size_t i = 0; i < 3; i++)
	{
		float x = RandomInRange<float>(-10.f,10.f);
		float z = RandomInRange<float>(-10.f,10.f);

		GameObject drone = gom.CreateGameObject();
		auto& mesh = drone.AddComponent<cMeshRenderer>("Models/Cube.fbx");
		mesh.SetMaterialPath("Materials/C64.json");

		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(x,0,z);
		transform.SetScale(.5f);
		drone.AddComponent<cActor>();
		computers[i] = drone.GetID();
	}

	const auto* playerPollingStation = new PollingStation(player,computers);
	for(size_t i = 0; i < 2; i++)
	{
		float x = RandomInRange<float>(-10.f,10.f);
		float z = RandomInRange<float>(-10.f,10.f);
		//Drone
		GameObject drone = gom.CreateGameObject();
		auto& mesh = drone.AddComponent<cMeshRenderer>("Models/C64.fbx");
		mesh.SetMaterialPath("Materials/C64Enemy.json");

		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(x,0,z);

		auto& actor = drone.AddComponent<cActor>();
		actor.SetController(new PollingController(*playerPollingStation));
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
