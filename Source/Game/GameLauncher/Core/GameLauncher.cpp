
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


#include <Tools/Utilities/AI/AgentSystem/AIEventManager.h>
#include <Tools/Utilities/AI/AgentSystem/EventController.h>
#include <Tools/Utilities/AI/AgentSystem/Actor.h>


#define Flashlight
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
		pLight.SetPower(5.0f);
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




	//World interfacing
	{
		//Drone
		GameObject drone = gom.CreateGameObject();
		drone.AddComponent<cMeshRenderer>("Models/C64.fbx");

		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(-10,0,0);
		transform.Rotate(-90,0,0); 
		 
		auto& actor = drone.AddComponent<cActor>();
		actor.GetActor()->SetController(new EventController());
	}

	{
		//Drone
		GameObject drone = gom.CreateGameObject();
		drone.AddComponent<cMeshRenderer>("Models/C64.fbx");

		auto& transform = drone.AddComponent<Transform>();
		transform.SetPosition(10,0,0);
		transform.Rotate(-90,0,0);


		auto& actor = drone.AddComponent<cActor>();
		actor.GetActor()->SetController(new EventController());
	}












	GLLogger.Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
	OPTICK_EVENT(); 
	//Movement1

















	//Other
	{
		for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>()) // TODO optimize this once you have a lot of lights and lightobjects
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	}

	Transform& pLight = GameObjectManager::GetInstance().GetWorldRoot().GetComponent<Transform>();
	pLight.Rotate(0,delta,0); 
}
