
// Exclude things we don't need from the Windows headers 
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>

#include "Editor.h"
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
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/Utilities/System/ThreadPool.hpp>

using json = nlohmann::json;

bool Editor::Initialize(HWND aHandle)
{
	MVLogger = Logger::Create("ModelViewer");
	ShowSplashScreen();

	ThreadPool::Get().Init();

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

	GameObjectManager::GetInstance().SetUpdatePriority<Transform>(ComponentManagerBase::UpdatePriority::Transform);
	GameObjectManager::GetInstance().SetUpdatePriority<cPhysics_Kinematic>(ComponentManagerBase::UpdatePriority::Physics);

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

	if(aMessage.message == WM_QUIT)
	{
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
	OPTICK_FRAME("MainThread")
		InputHandler::GetInstance().Update();

	if(IsGUIActive)
	{
		UpdateImGui();
		Update();
		Render();
	}
	else
	{
		Update();
		Render();
	}
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
void Editor::UpdateImGui()
{
	ImGui_ImplDX11_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplDX11_NewFrame",Optick::Category::UI);
	ImGui_ImplWin32_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplWin32_NewFrame",Optick::Category::UI);
	ImGui::NewFrame();
	OPTICK_CATEGORY("ImGui::NewFrame",Optick::Category::UI);
}
void Editor::Update()
{
	Timer::GetInstance().Update();
	GameObjectManager::GetInstance().Update();
	myGameLauncher.Update(Timer::GetInstance().GetDeltaTime());
}
void Editor::Render()
{
	GraphicsEngine::Get().BeginFrame();
	GraphicsEngine::Get().RenderFrame(0,0);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	GraphicsEngine::Get().EndFrame();
}

void Editor::TopBar()
{
}

RECT Editor::GetViewportRECT()
{
	static RECT ViewportRect;
	return ViewportRect;
}

Vector2<int> Editor::GetViewportResolution()
{
	RECT ViewportRect = GetViewportRECT();
	return Vector2<int>(
		ViewportRect.right - ViewportRect.left,
		ViewportRect.top - ViewportRect.bottom
	);
}

void Editor::ExpandWorldBounds(Sphere<float> sphere)
{
	if(myWorldBounds.ExpandSphere(sphere))
	{
		//MVLogger.Log("World bounds was expanded");
		for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
		{
			i.SetIsDirty(true);
			i.SetIsRendered(false);
		}
	} //REFACTOR if updated real time the world expansion will cause the light/shadow to lagg behind, use this to update camera position, 
	//REFACTOR not called on object moving outside worldbound causing same error as above
}
const Sphere<float>& Editor::GetWorldBounds()
{
	return myWorldBounds;
}