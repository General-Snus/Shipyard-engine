
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


#define _DEBUGDRAW
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
		pLight.SetPower(2.0f);
		pLight.BindDirectionToTransform(true);

		if(gom.GetAllComponents<BackgroundColor>().empty())
		{
			worldRoot.AddComponent<BackgroundColor>( Vector4f(1.0f,1.0f,1.0f,1.0f));
		}
	} 

	{

		myMesh.AddComponent<cSkeletalMeshRenderer>(L"Models/SK_C_TGA_Bro.fbx");
		myMesh.GetComponent<cSkeletalMeshRenderer>().SetMaterialPath(L"Materials/TGABroMaterial.json");

		auto& transform = myMesh.AddComponent<Transform>();
		transform.Rotate(0,-180,0);
		auto& animator = myMesh.AddComponent<cAnimator>(L"Animations/Locomotion/A_C_TGA_Bro_Walk.fbx");
		animator.AddAnimation(L"Animations/Locomotion/A_C_TGA_Bro_Run.fbx");
		animator.AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Brething.fbx");
		animator.AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Wave.fbx");
	}

	//Billboard
	{
		GameObject test = gom.CreateGameObject();
		test.AddComponent<cMeshRenderer>("Models/PlaneBillboard.fbx");
		test.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/BillboardSmiley.json");
		test.AddComponent<Transform>();
		test.GetComponent<Transform>().Rotate(-90,0,0);
		test.GetComponent<Transform>().SetPosition(100,0,0);
	}

#ifdef  ParticleSystemToggle
	{
		ParticleSystem& cmp = myCustomHandler.AddComponent<ParticleSystem>();
		auto& trans = myCustomHandler.AddComponent<Transform>();
		trans.Move(GlobalUp * 500.f);

		ParticleEmitterTemplate temp;
		temp.EmmiterSettings.StartSize = 100;
		temp.EmmiterSettings.EndSize = 1;
		temp.EmmiterSettings.StartColor = Vector4f(0,1,1,1);
		temp.EmmiterSettings.Acceleration = {0,-982.f,0}; // cm scale bad fix plz
		temp.EmmiterSettings.LifeTime = 3;
		temp.EmmiterSettings.SpawnRate = 1000;
		temp.EmmiterSettings.MaxParticles = 5000;

		temp.EmmiterSettings.EndColor = Vector4f(1,0,0,1);
		temp.EmmiterSettings.StartVelocity = Vector3f(-1000.0,1000.0,0.0);
		cmp.AddEmitter(temp);

		temp.EmmiterSettings.EndColor = Vector4f(0,1,0,1);
		temp.EmmiterSettings.StartVelocity = Vector3f(1000.0,1000.0,0);
		cmp.AddEmitter(temp);

		temp.EmmiterSettings.EndColor = Vector4f(0,0,1,1);
		temp.EmmiterSettings.StartVelocity = Vector3f(0,1000.0,1000.0);
		cmp.AddEmitter(temp);

		temp.EmmiterSettings.EndColor = Vector4f(1,0,1,1);
		temp.EmmiterSettings.StartVelocity = Vector3f(0,1000.0,-1000.0);
		cmp.AddEmitter(temp);
	}
#endif //  ParticleSystem

	{
		GameObject test2 = gom.CreateGameObject();
		test2.AddComponent<cMeshRenderer>("Models/Buddha.fbx");
		test2.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/BuddhaMaterial.json");
		Transform& trans = test2.AddComponent<Transform>();
		trans.SetPosition(Vector3f(0,-100,1000));
		trans.Rotate(0,-180,0);
		trans.SetScale(5);
	}


	const float Radius = 1.0f;
	for(size_t i = 0; i < Radius; i++)
	{
		const float Radians = 2 * PI * (i / Radius);
		float y = sin(Radians) * Radius;
		float x = cos(Radians) * Radius; 

		GameObject Chest = gom.CreateGameObject();
		Transform& trans = Chest.AddComponent<Transform>();
		trans.SetRotation(0,-90 - Radians * RAD_TO_DEG,0);
		trans.SetPosition(x,0,y);

		Chest.AddComponent<cMeshRenderer>("Models/Chest.fbx");
		Chest.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/ChestMaterial.json");
	}

	{
		GameObject Box = gom.CreateGameObject();
		Box.AddComponent<cMeshRenderer>("Models/Cube.fbx");
		Box.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/CubeMaterial.json");
		Box.AddComponent<Transform>();
		Box.GetComponent<Transform>().SetPosition(700,0,0);
	}

	for(int i = 1; i < 5; i++)
	{
		GameObject gO = gom.CreateGameObject();
		gO.AddComponent<cSkeletalMeshRenderer>(L"Models/SK_C_TGA_Bro.fbx");
		gO.GetComponent<cSkeletalMeshRenderer>().SetMaterialPath(L"Materials/TGABroMaterial.json");
		gO.AddComponent<Transform>();
		gO.GetComponent<Transform>().SetPosition({-static_cast<float>(i) * 300.0f,0});
		gO.GetComponent<Transform>().Rotate(0,180.0f,0);
		gO.AddComponent<cAnimator>(L"Animations/Locomotion/A_C_TGA_Bro_Walk.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Locomotion/A_C_TGA_Bro_Run.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Brething.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Wave.fbx");
		gO.GetComponent<cAnimator>().SetPlayingAnimation(i - 1);
	}

	for(int i = 1; i < 10; i++)
	{
		GameObject spotLight = gom.CreateGameObject();
		spotLight.AddComponent<cLight>(eLightType::Spot);
		std::weak_ptr<SpotLight> ptr = spotLight.GetComponent<cLight>().GetData<SpotLight>();
		spotLight.AddComponent<Transform>();
		spotLight.GetComponent<Transform>().SetPosition({-static_cast<float>(i) * 300.0f + 300 ,500,0});
		spotLight.GetComponent<Transform>().Rotate(90,0,0);
		ptr.lock()->Color = {RandomInRange(0.0f,1.0f),RandomInRange(0.0f,1.0f),RandomInRange(0.0f,1.0f)};
		//ptr.lock()->Color = {1,1,1};
		ptr.lock()->Range = 1000.0f;
		ptr.lock()->Power = static_cast<float>(i) * 200.0f * Kilo;
		ptr.lock()->OuterConeAngle = static_cast<float>(i) * 18.0f * DEG_TO_RAD;
		ptr.lock()->InnerConeAngle = 1.0f * DEG_TO_RAD;
		spotLight.GetComponent<cLight>().BindDirectionToTransform(true);

	}

	for(size_t i = 0; i < 2; i++)
	{
		int x = RandomInRange<int>(-5000,5000);
		int z = RandomInRange<int>(-5000,5000);

		GameObject pointLight = gom.CreateGameObject();
		pointLight.AddComponent<Transform>();
		pointLight.GetComponent<Transform>().SetPosition({(float)x,100,(float)z});

		pointLight.AddComponent<cLight>(eLightType::Point);
		std::weak_ptr<PointLight> ptr = pointLight.GetComponent<cLight>().GetData<PointLight>();
		ptr.lock()->Color = {RandomInRange(0.0f,1.0f),RandomInRange(0.0f,1.0f),RandomInRange(0.0f,1.0f)};
		ptr.lock()->Range = 10000.0f;
		ptr.lock()->Power = 50.0f * Kilo;
		pointLight.GetComponent<cLight>().BindDirectionToTransform(true);
	}

	GameObject pointLight = gom.CreateGameObject();
	pointLight.AddComponent<Transform>();
	pointLight.GetComponent<Transform>().SetPosition({0,300,1000});

	pointLight.AddComponent<cLight>(eLightType::Point);
	cLight& ptr = pointLight.GetComponent<cLight>();
	ptr.SetColor({1,1,1});
	ptr.SetRange(1000.0f);
	ptr.SetPower(100.0f * Kilo);
	ptr.BindDirectionToTransform(true);

	{
		GameObject test3 = gom.CreateGameObject();
		test3.AddComponent<cMeshRenderer>("Models/SteelFloor.fbx");
		test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");
		test3.AddComponent<Transform>();
		test3.GetComponent<Transform>().SetPosition(0,-125,0);
	}
	GLLogger.Log("GameLauncher start");
} 

void GameLauncher::Update(float delta)
{
	OPTICK_EVENT();
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::NUMPAD1))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(0);
	}
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::NUMPAD2))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(1);
	}
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::NUMPAD3))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(2);
	}
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::NUMPAD4))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(3);
	}
	//if(InputHandler::GetInstance().IsKeyPressed((int)Keys::R))
	{
		for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	}
	Transform& pLight = GameObjectManager::GetInstance().GetWorldRoot().GetComponent<Transform>();
	pLight.Rotate(0,delta,0);
	myMesh.GetComponent<Transform>().Rotate(0,delta * 100,0);
	/*Transform* transform = myCustomHandler.TryGetComponent<Transform>();
	if(transform)
	{
		transform->Rotate(0,10 * delta,0);
	}*/


}
 