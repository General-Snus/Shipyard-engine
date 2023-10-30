
// Exclude things we don't need from the Windows headers
#include <Engine/AssetManager/AssetManager.pch.h>
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>

#include "Modelviewer.h"

#define WIN32_LEAN_AND_MEAN 
#include "Windows.h"

#include <string>
#include <stringapiset.h>
#include <filesystem>
#include "Windows/SplashWindow.h"  

#include <functional>
#include <fstream>
#include <streambuf>
#include <assert.h>

#include <Tools/ImGUI/imgui.h>
#include <Tools/ImGUI/imgui_impl_win32.h>
#include <Tools/ImGUI/imgui_impl_dx11.h>

#include <Tools/Utilities/Math.hpp>
#include <Tools/ThirdParty/nlohmann/json.hpp>  
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>

#define _DEBUGDRAW
#define Flashlight
#define ParticleSystemToggle

using json = nlohmann::json;

bool ModelViewer::Initialize(HINSTANCE aHInstance,SIZE aWindowSize,WNDPROC aWindowProcess,LPCWSTR aWindowTitle)
{
	MVLogger = Logger::Create("ModelViewer");
	myModuleHandle = aHInstance;

	constexpr LPCWSTR windowClassName = L"ModelViewerMainWindow";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = aWindowProcess;
	windowClass.hCursor = LoadCursor(nullptr,IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	// Then we use the class to create our window
	myMainWindowHandle = CreateWindow(
		windowClassName,                                // Classname
		aWindowTitle,                                    // Window Title
		WS_OVERLAPPEDWINDOW | WS_POPUP,    // Flags
		(GetSystemMetrics(SM_CXSCREEN) - aWindowSize.cx) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - aWindowSize.cy) / 2,
		aWindowSize.cx,
		aWindowSize.cy,
		nullptr,nullptr,nullptr,
		nullptr
	);

	ShowSplashScreen();

	// TODO: Here we should init the Graphics Engine.
	GraphicsEngine::Get().Initialize(myMainWindowHandle,true);
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(myMainWindowHandle);
	ImGui_ImplDX11_Init(RHI::Device.Get(),RHI::Context.Get());


	LoadScene();

	HideSplashScreen();

	return true;
}

int ModelViewer::Run()
{
	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));

	bool isRunning = true;

	while(isRunning)
	{
		while(PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
		{

			extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
			if(ImGui_ImplWin32_WndProcHandler(msg.hwnd,msg.message,msg.wParam,msg.lParam))
				return true;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if(msg.message == WM_QUIT)
			{
				SaveDataToJson();
				ImGui_ImplDX11_Shutdown();
				ImGui_ImplWin32_Shutdown();
				ImGui::DestroyContext();
				return 0;
			}

			CU::InputHandler::GetInstance().UpdateEvents(msg.message,msg.wParam,msg.lParam);
		}
		// REMEMBER!
		// The frame update for the game does NOT happen inside the PeekMessage loop.
		// This would cause the game to only update if there are messages and also run
		// the update several times per frame (once for each message).

		Update();
		CU::InputHandler::GetInstance().Update();
		CU::InputHandler::GetInstance().UpdateMouseInput(msg.message);
	}

	return 0;
}

void ModelViewer::ShowSplashScreen()
{
	if(!mySplashWindow)
	{
		mySplashWindow = new SplashWindow();
		mySplashWindow->Init(myModuleHandle);
	}
}

void ModelViewer::HideSplashScreen() const
{
	mySplashWindow->Close();
	delete mySplashWindow;
	ShowWindow(myMainWindowHandle,SW_SHOW);
	SetForegroundWindow(myMainWindowHandle);
}

void ModelViewer::LoadScene()
{
	JsonToSaveData();

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
		cLight& pLight = worldRoot.GetComponent<cLight>();
		pLight.SetColor(CU::Vector3<float>(1,1,1));
		pLight.SetPower(1.0f);
		pLight.SetDirection({0,-1,1});

		if(gom.GetAllComponents<BackgroundColor>().empty())
		{
			worldRoot.AddComponent<BackgroundColor>(CU::Vector4<float>(1.0f,1.0f,1.0f,1.0f));
		}
	}


	{
		
		myMesh.AddComponent<cSkeletalMeshRenderer>(L"Models/SK_C_TGA_Bro.fbx");
		myMesh.GetComponent<cSkeletalMeshRenderer>().SetMaterialPath(L"Materials/TGABroMaterial.json");

		Transform& transform = myMesh.AddComponent<Transform>();
		transform.Rotate(0,-180,0);
		myMesh.AddComponent<cAnimator>(L"Animations/Locomotion/A_C_TGA_Bro_Walk.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Locomotion/A_C_TGA_Bro_Run.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Brething.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Wave.fbx");
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


	{
		GameObject Chest = gom.CreateGameObject();
		Transform& trans = Chest.AddComponent<Transform>();
		trans.Rotate(0,-180,0);
		trans.SetPosition(300,0,0);

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
}

void ModelViewer::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	CommonUtilities::Timer::GetInstance().Update();
	UpdateScene();
	GraphicsEngine::Get().BeginFrame();
	GraphicsEngine::Get().RenderFrame(0,0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	GraphicsEngine::Get().EndFrame();
}

void ModelViewer::UpdateScene()
{
	float delta = CU::Timer::GetInstance().GetDeltaTime();
	GameObjectManager::GetInstance().Update();

	if(GetAsyncKeyState('1'))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(0);
	}
	if(GetAsyncKeyState('2'))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(1);
	}
	if(GetAsyncKeyState('3'))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(2);
	}
	if(GetAsyncKeyState('4'))
	{
		myMesh.GetComponent<cAnimator>().SetPlayingAnimation(3);
	}

	myMesh.GetComponent<Transform>().Rotate(0,delta * 100,0);
	/*Transform* transform = myCustomHandler.TryGetComponent<Transform>();
	if(transform)
	{
		transform->Rotate(0,10 * delta,0);
	}*/

	for(auto& data : mySaveData)
	{
		if(data.identifier == "CameraPos")
		{
			//float* a[3] = {&myCamera->GetTransform()(4,1),&myCamera->GetTransform()(4,2),&myCamera->GetTransform()(4,3)};
			//ImGui::InputFloat3("CameraPos",*a);
			//SaveToMemory(eSaveToJsonArgument::InputFloat3,"CameraPos",*a);
		}
	}
}


bool ModelViewer::SaveDataToJson() const
{
	std::string path = "myJson.json";
	std::ofstream o(path);

	nlohmann::json j;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<BackgroundColor>())
	{
		j.push_back((json)i);
	}

	o << std::setw(4) << j << std::endl;

	return true;
}
bool ModelViewer::JsonToSaveData() const
{
	std::string path = "myJson.json";
	if(!std::filesystem::exists(path))
	{
		return false;
	}
	std::ifstream i(path);
	assert(i.is_open());
	nlohmann::json json = nlohmann::json::parse(i);
	i.close();
	GameObjectManager& gom = GameObjectManager::GetInstance();
	GameObject newG = gom.CreateGameObject();

	for(nlohmann::json& components : json)
	{
		if(components["myComponentType"] == eComponentType::backgroundColor)
		{
			newG.AddComponent < BackgroundColor>();
			newG.GetComponent<BackgroundColor>().SetColor(
				{
				components["myColor.x"],
				components["myColor.y"],
				components["myColor.z"],
				components["myColor.w"]
				});
		}
	}
	return true;
}
bool ModelViewer::ContainData(SaveData<float>& data)
{
	for(SaveData<float>& i : mySaveData)
	{
		if(i.fnc == data.fnc && i.identifier == data.identifier)
		{
			i.arg = data.arg;
			return true;
		}
	}
	return false;
}
bool ModelViewer::SaveToMemory(eSaveToJsonArgument fnc,const std::string& identifier,void* arg)
{
	switch(fnc)
	{
	case eSaveToJsonArgument::InputFloat3:
	{
		const auto* x = (float*)arg;
		arg = (float*)arg + 1;
		const auto* y = (float*)arg;
		arg = (float*)arg + 1;
		const auto* z = (float*)arg;

		for(SaveData<float>& i : mySaveData)
		{
			if(i.fnc == (int)fnc && i.identifier == identifier)
			{
				i.arg[0] = *x;
				i.arg[1] = *y;
				i.arg[2] = *z;
				return true;
			}
		}

		SaveData<float> data;
		data.fnc = (int)fnc;
		data.identifier = identifier;
		data.arg = new float[3];
		data.arg[0] = (*x);
		data.arg[1] = (*y);
		data.arg[2] = (*z);
		mySaveData.push_back(data);
		return true;
	}
	case eSaveToJsonArgument::SaveBool:
	{
		auto* x = (bool*)arg;
		for(SaveData<float>& i : mySaveData)
		{
			if(i.fnc == (int)fnc && i.identifier == identifier)
			{
				i.arg[0] = *x;
				return true;
			}
		}

		SaveData<float> data;
		data.fnc = (int)fnc;
		data.identifier = identifier;
		data.arg = new float[1];
		data.arg[0] = (*x);
		mySaveData.push_back(data);
		return true;
	}
	default:
		std::cout << "SaveFunction can not handle this argument";
		return false;
		break;
	}
}
bool ModelViewer::SaveToMemory(SaveData<float>& arg)
{
	for(SaveData<float>& i : mySaveData)
	{
		if(i.fnc == arg.fnc && i.identifier == arg.identifier)
		{
			i.arg = arg.arg;
			return true;
		}
	}
	mySaveData.push_back(arg);
	return true;
}

void ModelViewer::ExpandWorldBounds(CU::Sphere<float> sphere)
{
	if(myWorldBounds.ExpandSphere(sphere))
	{
		MVLogger.Log("World bounds was expanded");
		for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}


	} //REFACTOR if updated real time the world expansion will cause the light/shadow to lagg behind, use this to update camera position, 
	//REFACTOR not called on object moving outside worldbound causing same error as above
}
const CU::Sphere<float>& ModelViewer::GetWorldBounds() const
{
	return myWorldBounds;
}