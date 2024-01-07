
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h> 

#include "GameLauncher.h"

#include <Tools/Logging/Logging.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/ThirdParty/nlohmann/json.hpp>   
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>

#include <Game/GameLauncher/TaskSpecificImplementation/DecicionTree/DecisionTreeController.h>
#include <Game/GameLauncher/TaskSpecificImplementation/StateMachine/StateMachineController.h>


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
		transform.SetPosition(0,25,-25);
		transform.SetRotation(45,0,0);

	}
	{
		GameObject worldRoot = gom.CreateGameObject();
		gom.SetLastGOAsWorld();

		worldRoot.AddComponent<FrameStatistics>();
		//worldRoot.AddComponent<RenderMode>();
		worldRoot.AddComponent<Skybox>();
		worldRoot.AddComponent<cLight>(eLightType::Directional);

		Transform& transform = worldRoot.AddComponent<Transform>();
		transform.SetRotation(0,45,-45);

		cLight& pLight = worldRoot.GetComponent<cLight>();
		pLight.SetColor(Vector3f(1,1,1));
		pLight.SetPower(1.0f);
		pLight.BindDirectionToTransform(true);

		//if(gom.GetAllComponents<BackgroundColor>().empty())
		//{
		//	worldRoot.AddComponent<BackgroundColor>(Vector4f(1.0f,1.0f,1.0f,1.0f));
		//}
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

	{
		auto& transform = myCustomHandler.AddComponent<Transform>();
		myCustomHandler.AddComponent<cMeshRenderer>("Models/Cube.fbx");
		transform.SetScale(5.f);
	}

	GLLogger.Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
	delta;
	OPTICK_EVENT();

	AIEventManager::Instance().Update();
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::K))
	{
		GraphicsEngine::Get().GetSettings().DebugRenderer_Active = !GraphicsEngine::Get().GetSettings().DebugRenderer_Active;
	}
	static float angle;
	angle += delta;
	constexpr float radius = 5.f;
	myCustomHandler.GetComponent<Transform>().SetPosition(radius * cos(angle),5,radius * sin(angle));

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
