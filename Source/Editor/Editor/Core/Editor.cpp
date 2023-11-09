
// Exclude things we don't need from the Windows headers 
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>

#include "Editor.h"

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

#include "../Windows/Window.h" 
#include <Engine/AssetManager/Objects/GameObjects/GameObject.h>
#include <Tools/Optick/src/optick.h>

using json = nlohmann::json;

bool Editor::Initialize(HWND aHandle)
{
	MVLogger = Logger::Create("ModelViewer");
	ShowSplashScreen(); 
	// TODO: Here we should init the Graphics Engine.
	GraphicsEngine::Get().Initialize(aHandle,true);
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aHandle);
	ImGui_ImplDX11_Init(RHI::Device.Get(),RHI::Context.Get());


	myGameLauncher.Init();
	myGameLauncher.Start();

	HideSplashScreen();
	return true;
}
void Editor::DoWinProc(const MSG& aMessage)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	if(ImGui_ImplWin32_WndProcHandler(aMessage.hwnd,aMessage.message,aMessage.wParam,aMessage.lParam))
	{
		return;
	}

	//TranslateMessage(&aMessage);
	//DispatchMessage(&aMessage);

	if(aMessage.message == WM_QUIT)
	{
		SaveDataToJson();
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		return;
	}

	InputHandler::GetInstance().UpdateEvents(aMessage.message,aMessage.wParam,aMessage.lParam);
	InputHandler::GetInstance().UpdateMouseInput(aMessage.message);
}

int	 Editor::Run()
{
	OPTICK_FRAME("MainThread");
	InputHandler::GetInstance().Update();

	Update();
	return 0;
}

void Editor::ShowSplashScreen()
{
	if(!mySplashWindow)
	{
		mySplashWindow = new SplashWindow();
		mySplashWindow->Init(Window::moduleHandler);
	}
}

void Editor::HideSplashScreen() const
{
	mySplashWindow->Close();
	delete mySplashWindow;
	ShowWindow(Window::windowHandler,SW_SHOW);
	SetForegroundWindow(Window::windowHandler);
}

void Editor::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Timer::GetInstance().Update();
	GameObjectManager::GetInstance().Update();
	myGameLauncher.Update(Timer::GetInstance().GetDeltaTime());

	GraphicsEngine::Get().BeginFrame();
	GraphicsEngine::Get().RenderFrame(0,0);

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); 
	GraphicsEngine::Get().EndFrame();
}

bool Editor::SaveDataToJson() const
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
bool Editor::JsonToSaveData() const
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
bool Editor::ContainData(SaveData<float>& data)
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
bool Editor::SaveToMemory(eSaveToJsonArgument fnc,const std::string& identifier,void* arg)
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
bool Editor::SaveToMemory(SaveData<float>& arg)
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

void Editor::ExpandWorldBounds(Sphere<float> sphere)
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
const Sphere<float>& Editor::GetWorldBounds() const
{
	return myWorldBounds;
}