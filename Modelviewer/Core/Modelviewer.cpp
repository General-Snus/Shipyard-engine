
// Exclude things we don't need from the Windows headers
#define WIN32_LEAN_AND_MEAN
#define PI 3.14f
#include "Modelviewer.h"

#include "GraphicsEngine/GraphicsEngine.h"
#include "Windows.h"

#include <string>
#include <stringapiset.h>

#include <filesystem>

#include "GraphicsEngine/InterOp/Helpers.h"
#include "Windows/SplashWindow.h" 

#include <AssetManager/GameObjectManager.h>
#include <AssetManager/Objects/GameObjects/GameObject.h>
#include <AssetManager/Objects/Components/ComponentManager.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Transform.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Animator.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Skybox.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/CameraComponent.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/DEBUGCOMPONENTS/BackgroundColor.h>
#include <AssetManager/AssetManager.h>
#include <functional>
#include <fstream>
#include <streambuf>
#include <assert.h>

#include <ImGUI/imgui.h>
#include <ImGUI/imgui_impl_win32.h>
#include <ImGUI/imgui_impl_dx11.h>

#include <ThirdParty/nlohmann/json.hpp> 
#include "Timer.h"


using json = nlohmann::json;

bool ModelViewer::Initialize(HINSTANCE aHInstance,SIZE aWindowSize,WNDPROC aWindowProcess,LPCWSTR aWindowTitle)
{
	myLogger = Logger::Create("ModelViewer");
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
				isRunning = false;
				return 0;
			}
		}
		// REMEMBER!
		// The frame update for the game does NOT happen inside the PeekMessage loop.
		// This would cause the game to only update if there are messages and also run
		// the update several times per frame (once for each message).

		Update();
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
	//myCamera = std::make_shared<MainCamera>();

	GameObjectManager& gom = GameObjectManager::GetInstance();

	GameObject camera = gom.CreateGameObject();
	camera.AddComponent<cCamera>();
	gom.SetLastGOAsCamera();

	GameObject worldRoot = gom.CreateGameObject();
	gom.SetLastGOAsWorld();
	worldRoot.AddComponent<Skybox>();

	{
		myMesh = gom.CreateGameObject();
		myMesh.AddComponent<cSkeletalMeshRenderer>(L"Models/SK_C_TGA_Bro.fbx");
		myMesh.GetComponent<cSkeletalMeshRenderer>().SetMaterialPath(L"Materials/TGABroMaterial.json");

		CU::Matrix4x4<float> aPosition;
		aPosition = aPosition * CU::Matrix4x4<float>::CreateRotationAroundY(-PI);
		myMesh.AddComponent<Transform>(aPosition);
		myMesh.AddComponent<cAnimator>(L"Animations/Locomotion/A_C_TGA_Bro_Walk.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Locomotion/A_C_TGA_Bro_Run.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Brething.fbx");
		myMesh.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Wave.fbx");
	}
	GameObject test = gom.CreateGameObject();
	test.AddComponent<cMeshRenderer>();
	test.AddComponent<Transform>();
	test.GetComponent<Transform>().GetTransform()(4,1) = 100;;

	{

		GameObject test2 = gom.CreateGameObject();
		test2.AddComponent<cMeshRenderer>("Models/Buddha.fbx");
		test2.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/BuddhaMaterial.json");
		CU::Matrix4x4<float> aPosition;
		aPosition = aPosition * CU::Matrix4x4<float>::CreateRotationAroundY(-PI);
		test2.AddComponent<Transform>(aPosition);
		test2.GetComponent<Transform>().GetTransform()(4,3) = 1000;;
		test2.GetComponent<Transform>().GetTransform()(4,2) = -100;;
	}


	{
		GameObject Chest = gom.CreateGameObject();
		CU::Matrix4x4<float> ChestPosition;
		ChestPosition = ChestPosition * CU::Matrix4x4<float>::CreateRotationAroundY(-PI);
		Chest.AddComponent<Transform>(ChestPosition);
		Chest.AddComponent<cMeshRenderer>("Models/Chest.fbx");
		Chest.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/ChestMaterial.json");
		Chest.GetComponent<Transform>().GetTransform()(4,1) = 300;
	}

	{
		GameObject Box = gom.CreateGameObject();
		Box.AddComponent<cMeshRenderer>("Models/Cube.fbx");
		Box.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/CubeMaterial.json");
		Box.AddComponent<Transform>();
		Box.GetComponent<Transform>().GetTransform()(4,1) = 700;
	}

	for(int i = 1; i < 5; i++)
	{
		GameObject gO = gom.CreateGameObject();
		gO.AddComponent<cSkeletalMeshRenderer>(L"Models/SK_C_TGA_Bro.fbx");
		gO.GetComponent<cSkeletalMeshRenderer>().SetMaterialPath(L"Materials/TGABroMaterial.json");
		CU::Matrix4x4<float> aPosition;
		aPosition(4,1) = (float)i * 100;
		aPosition = aPosition * CU::Matrix4x4<float>::CreateRotationAroundY(-PI);
		gO.AddComponent<Transform>(aPosition);
		gO.AddComponent<cAnimator>(L"Animations/Locomotion/A_C_TGA_Bro_Walk.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Locomotion/A_C_TGA_Bro_Run.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Brething.fbx");
		gO.GetComponent<cAnimator>().AddAnimation(L"Animations/Idle/A_C_TGA_Bro_Idle_Wave.fbx");
		gO.GetComponent<cAnimator>().SetPlayingAnimation(i - 1);
	}

	{
		GameObject pointLight = gom.CreateGameObject();
		PointLight pLight;
		pLight.Position = CU::Vector3<float>(0,0,0);
		pLight.Color = CU::Vector3<float>(1,1,1);
		pLight.Intensity = 100;
		pLight.Range = 1000;

		pointLight.AddComponent<cLight>(eLightType::Point);

	}


	if(gom.GetAllComponents<BackgroundColor>().size() == 0)
	{
		GameObject newG = gom.CreateGameObject();
		newG.AddComponent<BackgroundColor>(CU::Vector4<float>(1.0f,1.0f,1.0f,1.0f));
	}


	{
		GameObject test3 = gom.CreateGameObject();
		test3.AddComponent<cMeshRenderer>("Models/SteelFloor.fbx");
		test3.GetComponent<cMeshRenderer>().SetMaterialPath("Materials/SteelFloor.json");;
		test3.AddComponent<Transform>();
		test3.GetComponent<Transform>().GetTransform()(4,2) = -125;;

	}
}

void ModelViewer::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow(); // Show demo window! :)

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
	//myCamera->Update(CommonUtilities::Timer::GetInstance().GetDeltaTime());
	//myCamera->UpdatePositionVectors();
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

	//LIGHTS
	GraphicsEngine::Get().AddCommand<GfxCmd_SetLightBuffer>();




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


bool ModelViewer::SaveDataToJson()
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
bool ModelViewer::JsonToSaveData()
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
bool ModelViewer::SaveToMemory(eSaveToJsonArgument fnc,std::string identifier,void* arg)
{
	switch(fnc)
	{
	case eSaveToJsonArgument::InputFloat3:
	{
		float* x = (float*)arg;
		arg = (float*)arg + 1;
		float* y = (float*)arg;
		arg = (float*)arg + 1;
		float* z = (float*)arg;

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
		bool* x = (bool*)arg;
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
	return false;
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



static std::string WStringToString(const std::wstring& someString)
{
	const int sLength = static_cast<int>(someString.length());
	const int len = WideCharToMultiByte(CP_ACP,0,someString.c_str(),sLength,0,0,0,0);
	std::string result(len,L'\0');
	WideCharToMultiByte(CP_ACP,0,someString.c_str(),sLength,&result[0],len,0,0);
	return result;
}

static std::wstring StringToWString(const std::string& someString)
{
	const int sLength = static_cast<int>(someString.length());
	const int len = MultiByteToWideChar(CP_ACP,0,someString.c_str(),sLength,0,0);
	std::wstring result(len,L'\0');
	MultiByteToWideChar(CP_ACP,0,someString.c_str(),sLength,&result[0],len);
	return result;
}