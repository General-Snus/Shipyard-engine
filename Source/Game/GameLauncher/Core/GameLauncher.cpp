
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h> 

#include <Game/GameLauncher/TaskSpecificImplementation/DecicionTree/DecisionTreeController.h>
#include <Game/GameLauncher/TaskSpecificImplementation/StateMachine/StateMachineController.h>
#include <Tools/Logging/Logging.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/ThirdParty/nlohmann/json.hpp>   
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>
#include "GameLauncher.h"

#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#include <fstream>
#include <iostream>
#include <sstream>



using json = nlohmann::json;

void GameLauncher::Init()
{
	GLLogger.Log("GameLauncher Init");
}

bool SaveTest(std::vector<GameObject> gameObjectsToSave,const std::filesystem::path& path)
{
	AMLogger.Log("\n\nSaving Gameobjects");
	std::ofstream file(path.string(),std::ios_base::binary);
	if (!file.is_open())
	{
		return false;
	}
	int a = static_cast<int>(gameObjectsToSave.size());
	file.write((char*)&a,sizeof(a));

	for (auto& i : gameObjectsToSave)
	{
		const Vector3f& position = i.GetComponent<Transform>().GetPosition();
		std::string meshPath = i.GetComponent<cMeshRenderer>().GetRawMesh()->GetAssetPath().string();
		int strLength = static_cast<int>(meshPath.length());
		unsigned int id = i.GetID();

		file.write((char*)&id,sizeof(id));
		file.write((char*)&position,sizeof(position));
		file.write((char*)&strLength,sizeof(strLength));
		file.write(&meshPath[0],strLength);

		AMLogger.Log("Saved: " + std::to_string(id));
	}
	file.close();
	return true;
}

std::vector<GameObject> LoadTest(const std::filesystem::path& path)
{
	std::vector<GameObject> gameObjectsToSave;
	std::ifstream file(path.string(),std::ios_base::binary);
	if (!file.is_open())
	{
		return gameObjectsToSave;
	}

	int size = 0;
	file.read((char*)&size,sizeof(int));
	gameObjectsToSave.resize(size);
	AMLogger.Log("\n\nLoading Gameobjects " + std::to_string(size));

	for (auto& i : gameObjectsToSave)
	{
		SY::UUID uuid;
		Vector3f position;
		std::string meshPath;
		int strLength = 0;

		file.read((char*)&uuid,sizeof(uuid));
		file.read((char*)&position,sizeof(position));

		file.read((char*)&strLength,sizeof(strLength));
		meshPath.resize(strLength);
		file.read(&meshPath[0],strLength);


		GameObjectManager::Get().CustomOrderUpdate();
		i = GameObjectManager::Get().CreateGameObjectAt(uuid);
		auto& transform = i.AddComponent<Transform>();
		transform.SetPosition(position);
		i.AddComponent<cMeshRenderer>(meshPath,true);
		i.AddComponent<cPhysXDynamicBody>();
		AMLogger.Log("Loaded: " + std::to_string(uuid));
	}
	file.close();
	return gameObjectsToSave;
}

void GameLauncher::GenerateNewRandomCubes()
{
	std::string arr[3] = { "Models/Cube.fbx","Models/CubeHoled.fbx","Models/SteelFloor.fbx" };
	for (size_t i = 0; i < 10; i++)
	{
		vectorOfGameObjects.push_back(GameObjectManager::Get().CreateGameObject());
		GameObject vectorObject = vectorOfGameObjects.back();
		vectorObject.AddComponent<cMeshRenderer>(arr[rand() % 3]);
		auto& transform = vectorObject.AddComponent<Transform>();

		Vector3f position = { RandomEngine::RandomInRange(-20.f,20.f),RandomEngine::RandomInRange(0.f,20.f),RandomEngine::RandomInRange(-20.f,20.f) };
		transform.SetPosition(position);
		transform.SetScale(1.f);

		vectorObject.AddComponent<cPhysXDynamicBody>();

		AMLogger.Log("Created: " + std::to_string(vectorObject.GetID()));
	}
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
		//worldRoot.AddComponent<Skybox>();
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
		GameObject floor = gom.CreateGameObject();
		auto& transform = floor.AddComponent<Transform>();
		transform.SetPosition(0,-20.0f,0);
		transform.SetRotation(90,0.f,0.f);
		transform.SetScale(50.f);
		transform.SetGizmo(false);

		floor.AddComponent<cMeshRenderer>("Models/MeshCanyon.fbx");
		//test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");

		auto& collider = floor.AddComponent<cCollider>();
		collider.SetColliderType<ColliderAssetPlanar>("Models/ColliderMesh.fbx");



		floor.AddComponent<cPhysXStaticBody>();
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
	if (std::filesystem::exists("GameObjectSaveFile.SaveFiles"))
	{
		vectorOfGameObjects = LoadTest("GameObjectSaveFile.SaveFiles");
	}
	else
	{
		GenerateNewRandomCubes();
	}

	GLLogger.Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{
	delta;
	OPTICK_EVENT();
	AIEventManager::Instance().Update();
	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::K))
	{
		GraphicsEngine::Get().GetSettings().DebugRenderer_Active = !GraphicsEngine::Get().GetSettings().DebugRenderer_Active;
	}

	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::F4))
	{
		while (vectorOfGameObjects.size())
		{
			GameObjectManager::Get().DeleteGameObject(vectorOfGameObjects.back());
			vectorOfGameObjects.pop_back();
		}
		GameObjectManager::Get().CustomOrderUpdate();
	}

	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::F5))
	{
		SaveTest(vectorOfGameObjects,"GameObjectSaveFile.SaveFiles");
	}

	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::F6))
	{
		if (std::filesystem::exists("GameObjectSaveFile.SaveFiles"))
		{
			while (vectorOfGameObjects.size())
			{
				GameObjectManager::Get().DeleteGameObject(vectorOfGameObjects.back());
				vectorOfGameObjects.pop_back();
			}
			GameObjectManager::Get().CustomOrderUpdate();
			vectorOfGameObjects = LoadTest("GameObjectSaveFile.SaveFiles");
		}
	}

	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::R))
	{
		GenerateNewRandomCubes();
	}

	//Other
	{
		for (auto& i : GameObjectManager::Get().GetAllComponents<cLight>()) // TODO optimize this once you have a lot of lights and lightobjects
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
		}

	//Transform& pLight = GameObjectManager::GetInstance().GetWorldRoot().GetComponent<Transform>();
	//pLight.Rotate(0,delta,0);
	}
