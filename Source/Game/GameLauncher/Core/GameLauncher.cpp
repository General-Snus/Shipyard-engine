#include "ShipyardEngine.pch.h"

#include "GameLauncher.h"

#include <UserComponent.h>
#include "Editor/Editor/Windows/EditorWindows/Viewport.h"
#include "Engine/PersistentSystems/Physics/Raycast.h"
#include "PillarGame/PillarGameComponents.h"
#include <DirectX/XTK/Inc/SimpleMath.h>

#include "Tools/ImGui/crude_json.h"
#include "Tools/Utilities/LinearAlgebra/Easing.h"

extern "C" {
	inline GAME_API GameLauncher* entrypointMain() {
		return new YourGameLauncher();
	}
}

extern "C" {
	inline GAME_API void exitPoint(HMODULE handle) {
		FreeLibraryAndExitThread(handle,0);
	}
}

void YourGameLauncher::Init() {
	OPTICK_EVENT();
	{
		{
			GameObject SkySphere = GameObject::Create("SkySphere");
			auto& mesh = SkySphere.AddComponent<MeshRenderer>("Materials/MaterialPreviewMesh.fbx");
			mesh.SetMaterialPath("Materials/SkySphere.json");
			SkySphere.transform().SetScale(-100000,-100000,-100000);
		}

		{
			GameObject worldRoot = GameObject::Create();
			Scene::activeManager().SetLastGOAsWorld();

			worldRoot.SetName("SkyLight");
			worldRoot.transform().SetRotation(45,45,0);
			auto& light = worldRoot.AddComponent<Light>(eLightType::Directional);
			light.SetIsShadowCaster(true);
			light.SetColor("White");
			light.SetPower(4.0f);
			light.BindDirectionToTransform(true);
		}

		constexpr float pillarRadius = 1.0f;
		auto            groundPos = Vector3f(0,-.5f,0);
		SpawnGround(groundPos);

		auto pillarTransform = SpawnPillar(groundPos);
		SpawnHooks(50,pillarRadius,pillarTransform);
		SpawnPlayer(0,pillarRadius,pillarTransform);
	}
}

void YourGameLauncher::Start() {
	OPTICK_EVENT();
	LOGGER.Log("GameLauncher start");
	GraphicsEngineInstance.debugDrawer.AddDebugBox({-1, -1, -1},{1, 1, 1});
}

#pragma optimize( "", off ) 

void YourGameLauncher::Update(float delta) {
	OPTICK_EVENT();
	UNREFERENCED_PARAMETER(delta);
	auto& manager = Scene::activeManager();

	static Vector3f lerpPos;
	static Vector3f lerpRot;
	for(auto& element : manager.GetAllComponents<PlayerComponent>()) {
		if(element.currentHook) {
			element.timeSinceHook += delta;
			auto percentage = element.timeSinceHook / element.hookCooldown;

			if(percentage < 1.0f) {
				auto position = element.transform().GetPosition();
				position.y = lerp(lerpPos.y,element.currentHook.transform().GetPosition().y,percentage);
				element.transform().SetPosition(position);

				auto rotation = element.transform().euler();
				auto hookPosition = element.currentHook.transform().GetPosition();

				const auto towerCenter2d = Vector2(position.swizzle<VectorIndices::x,VectorIndices::z>()); // Player parent origin
				const auto hook2dPosition = Vector2(hookPosition.swizzle<VectorIndices::x,VectorIndices::z>());
				const auto hookToCenter = hook2dPosition - towerCenter2d;
				const auto q = Quaternionf::LookAt(hookToCenter,Vector3f::up());

				rotation.y = lerp(lerpRot.y,q.GetEulerAngles().y,percentage);
				element.transform().SetRotation(rotation); 
				return;
			}
		}


		if(Input.IsKeyHeld(Keys::RIGHT)) {
			element.transform().Rotate(0,-50.0f * delta,0);
		}

		if(Input.IsKeyHeld(Keys::LEFT)) {
			element.transform().Rotate(0,50.0f * delta,0);
		}

		if(Input.IsKeyPressed(Keys::SPACE)) {
			Physics::RaycastHit hit;
			const auto& cameraTransform = manager.GetCamera().transform();
			auto& camera = manager.GetCamera().GetComponent<Camera>();

			const auto coord = EDITOR_INSTANCE.GetMainViewport()->getCursorInWindowPostion();
			const auto position = cameraTransform.GetPosition(WORLD);
			const auto direction = camera.GetPointerDirection(coord);
			const bool raycastHit = Raycast(position,direction,hit); 
			auto* hook = raycastHit ? hit.objectHit.TryGetComponent<HookComponent>() : nullptr;

			if(hook && !hook->hasConnection) {
				GraphicsEngineInstance.debugDrawer.AddDebugLine(position,hit.point,{0.0f,1.0f,0},1.0f);
				LOGGER.Log(hit.objectHit.GetName());
				if(element.currentHook) {
					auto& currentHook = element.currentHook.GetComponent<HookComponent>();
					currentHook.hasConnection = false;
					currentHook.connection = GameObject();
				}
				element.timeSinceHook = 0;
				element.currentHook = hit.objectHit;
				hook->connection = element.gameObject();
				hook->hasConnection = true;

				lerpPos = element.transform().GetPosition();
				lerpRot = element.transform().euler();
				return;
			} else {
				GraphicsEngineInstance.debugDrawer.AddDebugLine(position,direction + position,{1.0f,0,0},1.0f);
			}
		}
	}
}

#pragma optimize( "", on )

void YourGameLauncher::SyncServices(ServiceLocator& serviceLocator) {
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
	switch(reason) {
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
