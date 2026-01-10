#include "ShipyardEngine.pch.h"

#include "GameLauncher.h"	

#include <UserComponent.h>
#include "Editor/Editor/Windows/EditorWindows/Viewport.h"
#include "Engine/PersistentSystems/Physics/Raycast.h" 
#include "NetworkingGame/BallBouncerGame.h" 
#include "Tools/Utilities/Math.hpp" 

#include "Tools/ImGui/crude_json.h"
#include "Tools/Utilities/LinearAlgebra/Easing.h"
#include "Engine\PersistentSystems\Networking\NetworkRunner.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include "Engine/AssetManager/ComponentSystem/Components/LightComponent.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"

extern "C" {
	inline GAME_API GameLauncher* entrypointMain()
	{
		return new YourGameLauncher();
	}
}

extern "C" {
	inline GAME_API void exitPoint(HMODULE handle)
	{
		FreeLibraryAndExitThread(handle, 0);
	}
}

void YourGameLauncher::Init()
{
	OPTICK_EVENT();
}

void YourGameLauncher::Start()
{
	OPTICK_EVENT();
	{
		GameObject SkySphere = GameObject::Create("SkySphere");
		auto& mesh = SkySphere.AddComponent<MeshRenderer>("Materials/MaterialPreviewMesh.fbx");
		mesh.SetMaterialPath("Materials/SkySphere.json");
		SkySphere.transform().SetScale(-100000, -100000, -100000);
	}

	{
		GameObject worldRoot = GameObject::Create();
		Scene::activeManager().SetLastGOAsWorld();

		worldRoot.SetName("SkyLight");
		worldRoot.transform().SetRotation(45, 45, 0);
		auto& light = worldRoot.AddComponent<Light>(eLightType::Directional);
		light.SetIsShadowCaster(true);
		light.SetColor("White");
		light.SetPower(4.0f);
		light.BindDirectionToTransform(true);
	}

	{
		GameObject SceneCamera = GameObject::Create("SceneCamera");
		auto& camera = SceneCamera.AddComponent<Camera>();
		Scene::activeManager().SetLastGOAsCamera();

		camera.isOrtho = true;
		camera.orthoRect = { 110,100 };
		camera.UpdateProjection(); //yeezz

		camera.transform().SetPosition({ 0,10,0 });
		camera.transform().SetRotation({ 90,0,0 });
	}

	this->arena = BallEradicationGame::MakeArena(Vector3f::zero(), rect);
	arena.AddComponent<BallGameController>();


	if (Runner.IsServer)
	{
		this->player = BallEradicationGame::MakePlayer(Vector3f::zero());
		player.GetComponent<NetworkObject>().RegisterAsPlayer();
	}
	else
	{
		Runner.layer.CallbackOnPlayerCreated([&](NetworkedId id)
		{
			this->player = BallEradicationGame::MakePlayer(Vector3f::zero(), id);
			player.GetComponent<NetworkObject>().RegisterAsPlayer();
		});
	}

}


void YourGameLauncher::Update(float delta)
{
	OPTICK_EVENT();
	UNREFERENCED_PARAMETER(delta);
	//This checks that you are the server or that you are in a context that allows it
	if (!Runner.HasStateAuthority)
	{
		return;
	}

	auto& manager = Scene::activeManager();
	const auto& controller = arena.GetComponent<BallGameController>();
	for (auto& ball : manager.GetAllComponents<BallTag>())
	{
		//Check collisions 
		const auto& sphere1 = ball.GetComponent<Collider>().GetColliderAssetOfType<ColliderAssetSphere>()->sphere();
		for (const auto& ClientPlayers : Runner.AllPlayers())
		{
			const auto& sphere2 = ClientPlayers.GetComponent<Collider>().GetColliderAssetOfType<ColliderAssetSphere>()->sphere();
			if ((ball.transform().GetPosition() - ClientPlayers.transform().GetPosition()).Length() < sphere1.GetRadius() + sphere2.GetRadius())
			{
				manager.DeleteGameObject(ball.gameObject());
				break;
			}

		}

		for (const auto& otherBall : manager.GetAllComponents<BallTag>())
		{
			if (ball.gameObject() == otherBall.gameObject())
			{
				continue;
			}

			const auto& sphere2 = otherBall.GetComponent<Collider>().GetColliderAssetOfType<ColliderAssetSphere>()->sphere();
			if ((ball.transform().GetPosition() - otherBall.transform().GetPosition()).Length() < sphere1.GetRadius() + sphere2.GetRadius())
			{
				manager.DeleteGameObject(otherBall.gameObject());
				manager.DeleteGameObject(ball.gameObject());
				break;
			}
		}
		auto ballPosition = ball.transform().GetPosition();
		auto& kinematic = ball.GetComponent<cPhysics_Kinematic>();

		float speedup = 1.f + (delta);
		if (ballPosition.x > rect.x && kinematic.ph_velocity.x > 0)
		{
			kinematic.ph_velocity.x = std::copysign(kinematic.ph_velocity.x, -1.0f);
		}

		if (ballPosition.x < -rect.x && kinematic.ph_velocity.x < 0)
		{
			kinematic.ph_velocity.x = std::copysign(kinematic.ph_velocity.x, 1.0f);
		}

		if (ballPosition.z > rect.z && kinematic.ph_velocity.z > 0)
		{
			kinematic.ph_velocity.z = std::copysign(kinematic.ph_velocity.z, -1.0f);
		}

		if (ballPosition.z < -rect.z && kinematic.ph_velocity.z < 0)
		{
			kinematic.ph_velocity.z = std::copysign(kinematic.ph_velocity.z, 1.0f);
		}

		kinematic.ph_velocity *= speedup;
	}

	auto ballDiff = controller.maxBallsInGame - manager.GetAllComponents<BallTag>().size();
	ballSpawnTimer -= delta;
	if (ballDiff > 0 && ballSpawnTimer < 0)
	{
		ballSpawnTimer = controller.ballSpawnCooldown;
		Vector3f position;
		position.x = Math::RandomEngine::randomInRange(-rect.x, rect.x);
		position.z = Math::RandomEngine::randomInRange(-rect.z, rect.z);
		BallEradicationGame::MakeBall(position);
	}
}

void YourGameLauncher::SyncServices(ServiceLocator& serviceLocator)
{
	ServiceLocator::SyncInstances(serviceLocator);
	InitializeOnNewContext(serviceLocator.GetService<ImGui::ImGuiContextHolder>());
};

extern "C" BOOL WINAPI DllMain(const HINSTANCE instance, // handle to DLL module
	const DWORD     reason,   // reason for calling function
	const LPVOID    reserved) // reserved
{
	instance;
	reserved;
	// Perform actions based on the reason for calling.
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
