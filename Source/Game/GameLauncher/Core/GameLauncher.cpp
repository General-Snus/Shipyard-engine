
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h> 

#include <Game/GameLauncher/TaskSpecificImplementation/DecicionTree/DecisionTreeController.h>
#include <Game/GameLauncher/TaskSpecificImplementation/StateMachine/StateMachineController.h>
#include <Tools/Logging/Logging.h>
#include <Tools/Optick/include/optick.h>
#include <Tools/ThirdParty/nlohmann/json.hpp>   
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>
#include "GameLauncher.h"

#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "GraphicsEngine.h"
#include "Input/EnumKeys.h"

using json = nlohmann::json;

void GameLauncher::Init()
{
}

bool SaveTest(std::vector<GameObject> gameObjectsToSave,const std::filesystem::path& path)
{
	Logger::Log("\n\nSaving Gameobjects");
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

		Logger::Log("Saved: " + std::to_string(id));
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
	Logger::Log("\n\nLoading Gameobjects " + std::to_string(size));

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
		Logger::Log("Loaded: " + std::to_string(uuid));
	}
	file.close();
	return gameObjectsToSave;
}

void GameLauncher::GenerateNewRandomCubes()
{
	const float range = 50.f;
	std::string arr[3] = { "Models/Cube.fbx","Models/CubeHoled.fbx","Models/SteelFloor.fbx" };
	for (size_t i = 0; i < 10; i++)
	{
		vectorOfGameObjects.push_back(GameObjectManager::Get().CreateGameObject());
		GameObject vectorObject = vectorOfGameObjects.back();
		vectorObject.AddComponent<cMeshRenderer>(arr[rand() % 3]);
		auto& transform = vectorObject.AddComponent<Transform>();

		Vector3f position = { RandomEngine::RandomInRange(-range,range),RandomEngine::RandomInRange(0.f,2 * range),RandomEngine::RandomInRange(-range,range) };
		transform.SetPosition(position);
		transform.SetScale(1.f);

		vectorObject.AddComponent<cPhysXDynamicBody>();

		//Logger::Log("Created: " + std::to_string(vectorObject.GetID()));
	}
}

void GameLauncher::Start()
{
	GameObjectManager& gom = GameObjectManager::Get();
#pragma region BaseSetup

	myMesh = gom.CreateGameObject();
	myMesh.SetName("MyMesh");
	{
		GameObject camera = gom.CreateGameObject();
		camera.SetName("Camera");
		auto& cameraComponent = camera.AddComponent<cCamera>();
		gom.SetLastGOAsCamera();
		cameraComponent.SetActive(true);
		auto& transform = camera.AddComponent<Transform>();
		transform.SetPosition(-10,27,0);
		transform.SetRotation(0,90,0);
	}

	{
		GameObject worldRoot = gom.CreateGameObject();
		gom.SetLastGOAsWorld();
		worldRoot.SetName("WordRoot");
		worldRoot.AddComponent<FrameStatistics>();
		worldRoot.AddComponent<RenderMode>();
		//worldRoot.AddComponent<Skybox>();
		Transform& transform = worldRoot.AddComponent<Transform>();
		transform.SetRotation(80,0,0);
		cLight& pLight = worldRoot.AddComponent<cLight>(eLightType::Directional);
		worldRoot.AddComponent<cMeshRenderer>("Models/Cube.fbx");

		pLight.SetColor(Vector3f(1,1,1));
		pLight.SetPower(2.0f);
		pLight.BindDirectionToTransform(true);
		//if(gom.GetAllComponents<BackgroundColor>().empty())
		//{
		//	worldRoot.AddComponent<BackgroundColor>(Vector4f(1.0f,1.0f,1.0f,1.0f));
		//}
	}

	{
		GameObject floor = gom.CreateGameObject();
		floor.SetName("Floor");

		auto& transform = floor.AddComponent<Transform>();
		transform.SetPosition(0,-0.0f,0);
		transform.SetRotation(0,0.f,0.f);
		transform.SetScale(500.f,20.f,500.f);
		transform.SetGizmo(false);
		floor.SetActive(false);
		floor.AddComponent<cMeshRenderer>("Models/Cube.fbx");
#if PHYSX
		auto& collider = floor.AddComponent<cCollider>();
		collider.SetColliderType<ColliderAssetAABB>();
		floor.AddComponent<cPhysXStaticBody>();
#endif 
	}
#if true // Sponza
	{
		GameObject sponza = gom.CreateGameObject();
		sponza.SetName("Sponza");

		sponza.AddComponent<cMeshRenderer>("Models/Sponza/Sponza.fbx");
		//sponza.AddComponent<cMeshRenderer>("Models/Sponza/Sponza3Intel.fbx");
		auto& transform = sponza.AddComponent<Transform>();
		transform.SetPosition(0,25,0);
		transform.SetScale(.01f);
		transform.SetGizmo(false);
	}
#endif 
	{
		myCustomHandler = gom.CreateGameObject();
		myCustomHandler.SetName("CustomHandler");

		auto& transform = myCustomHandler.AddComponent<Transform>();
		auto& light = myCustomHandler.AddComponent<cLight>(eLightType::Point);
		transform.SetPosition(0,30,-4);
		light.BindDirectionToTransform(true);
		light.SetColor({ .5f,.5f,1 });
		light.SetPower(10);
		light.SetIsShadowCaster(false);
		light.SetRange(4.5f);
	}

	{
		myCustomHandler2 = gom.CreateGameObject();
		myCustomHandler.SetName("CustomHandler2");

		auto& transform = myCustomHandler2.AddComponent<Transform>();
		auto& light = myCustomHandler2.AddComponent<cLight>(eLightType::Point);
		transform.SetPosition(0,30,4);
		light.BindDirectionToTransform(true);
		light.SetIsShadowCaster(false);
		light.SetColor({ 1,.5f,.5f });
		light.SetPower(10);
		light.SetRange(4.5f);
	}
	{
		GameObject buddha = gom.CreateGameObject();
		buddha.SetName("buddha");

		buddha.AddComponent<cMeshRenderer>("Models/Buddha.fbx");
		buddha.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/BuddhaMaterial.json");
		auto& transform = buddha.AddComponent<Transform>();
		transform.SetPosition(0,25,0);
		transform.Rotate(0,90,0);
		transform.SetScale(0.5f,.5f,.5f);
		transform.SetGizmo(false);
	}
	{
		//for (int x = 0; x < 4; x++)
		//{
		//	for (int y = 0; y < 1; y++)
		//	{
		//		for (int z = 0; z < 4; z++)
		//		{
		//			GameObject p7 = gom.CreateGameObject();
		//			p7.AddComponent<cMeshRenderer>("Models/L_Main.FBX");
		//			//test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");
		//			auto& transform = p7.AddComponent<Transform>();
		//			transform.SetPosition(500.f * x,150.f + 500.f * y,500.f * z);
		//			transform.SetRotation(90,0,0);
		//			transform.SetGizmo(false);
		//		}
		//	}
		//}
	}

#pragma endregion

#if PHYSX 
	if (std::filesystem::exists("GameObjectSaveFile.SaveFiles"))
	{
		//vectorOfGameObjects = LoadTest("GameObjectSaveFile.SaveFiles");
	}
	else
	{
		GenerateNewRandomCubes();
	}
#endif
	Logger::Log("GameLauncher start");
}

void GameLauncher::Update(float delta)
{

	if (InputHandler::GetInstance().IsKeyPressed(static_cast<unsigned>(Keys::I)))
	{
		Logger::Log(std::to_string(1.f / delta));
	}
	delta;
	OPTICK_EVENT();
	AIEventManager::Instance().Update();
	if (InputHandler::GetInstance().IsKeyPressed(static_cast<int>(Keys::K)))
	{
		GraphicsEngine::Get().GetSettings().DebugRenderer_Active = !GraphicsEngine::Get().GetSettings().DebugRenderer_Active;
	}

	if (InputHandler::GetInstance().IsKeyPressed(static_cast<int>(Keys::F5)))
	{
		Editor::GetApplicationState().filter = DebugFilter::NoFilter;
	}

	if (InputHandler::GetInstance().IsKeyPressed(static_cast<int>(Keys::F6)))
	{
		Editor::GetApplicationState().filter = static_cast<DebugFilter>((static_cast<int>(Editor::GetApplicationState().filter) + 1) % static_cast<int>(DebugFilter::count));
	}
#if PHYSX
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
#endif

	//Other
	{
		for (auto& i : GameObjectManager::Get().GetAllComponents<cLight>()) // TODO optimize this once you have a lot of lights and lightobjects
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	}

	{
		auto& transform = myCustomHandler.GetComponent<Transform>();
		auto position = transform.GetPosition();
		if (position.x > 12.f)
		{
			direction = -1.f;
		}
		if (position.x < -12.f)
		{
			direction = 1.f;
		}
		transform.Move(5.f * direction * delta,0,0);
	}

	{
		auto& transform = myCustomHandler2.GetComponent<Transform>();
		auto position = transform.GetPosition();
		if (position.x > 12.f)
		{
			direction2 = -1.f;
		}
		if (position.x < -12.f)
		{
			direction2 = 1.f;
		}
		transform.Move(5.f * direction * delta,0,0);
	}



	Transform& pLight = GameObjectManager::Get().GetWorldRoot().GetComponent<Transform>();
	constexpr float rotSpeed = 25.f;
	if (InputHandler::GetInstance().IsKeyHeld(static_cast<int>(Keys::NUMPAD6)))
	{
		pLight.Rotate(rotSpeed * delta,0,0);
	}
	if (InputHandler::GetInstance().IsKeyHeld(static_cast<int>(Keys::NUMPAD3)))
	{
		pLight.Rotate(-rotSpeed * delta,0,0);
	}
}
