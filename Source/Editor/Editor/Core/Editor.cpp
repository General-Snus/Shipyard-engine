#define NOMINMAX
#include <assert.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXDynamicBody.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXStaticBody.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h> 
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <streambuf> 
#include <string>
#include <stringapiset.h>
#include <Tools/ImGUI/ImGUI/imgui.h>
#include <Tools/ImGUI/ImGUI/imgui_impl_dx11.h>
#include <Tools/ImGUI/ImGUI/imgui_impl_win32.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/System/ThreadPool.hpp>
#include <Windows/EditorWindows/ChainGraph/GraphTool.h>
#include "../Windows/Window.h" 
#include "Editor.h" 
#include "Windows.h"
#include "Windows/SplashWindow.h"

#if PHYSX 
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#endif // PHYSX 0

using json = nlohmann::json;

bool Editor::Initialize(HWND aHandle)
{
	MVLogger = Logger::Create("ModelViewer");
	GELogger = Logger::Create("GraphicsEngine");
	GetWindowRect(Window::windowHandler,&ViewportRect);



	ShowSplashScreen();
	ThreadPool::Get().Init();

#ifdef _DEBUG
	GraphicsEngine::Get().Initialize(aHandle,true);
#else
	GraphicsEngine::Get().Initialize(aHandle,false);
#endif // Release


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch 
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(aHandle);
	ImGui_ImplDX11_Init(RHI::Device.Get(),RHI::Context.Get());

#if PHYSX
	Shipyard_PhysX::Get().InitializePhysx();
#endif // PHYSX 0

	myGameLauncher.Init();
	myGameLauncher.Start();

	GameObjectManager::Get().SetUpdatePriority<Transform>(ComponentManagerBase::UpdatePriority::Transform);
	GameObjectManager::Get().SetUpdatePriority<cPhysics_Kinematic>(ComponentManagerBase::UpdatePriority::Physics);
	GameObjectManager::Get().SetUpdatePriority<cPhysXDynamicBody>(ComponentManagerBase::UpdatePriority::Physics);
	//Force no write to thread after this?

	HideSplashScreen();
#if UseScriptGraph
	ScriptEditor = Graph::GraphTool::Get().GetScriptingEditor();
	ScriptEditor->Init();
#endif
	return true;
}
void Editor::DoWinProc(const MSG& aMessage)
{
	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(aMessage.hwnd,aMessage.message,aMessage.wParam,aMessage.lParam))
	{
		return;
	}

	if (aMessage.message == WM_QUIT)
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
	OPTICK_FRAME("MainThread");
	InputHandler::GetInstance().Update();

	if (IsGUIActive)
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
	if (!mySplashWindow)
	{
		mySplashWindow = std::make_unique<SplashWindow>();
		mySplashWindow->Init(Window::moduleHandler);
	}
}

void Editor::HideSplashScreen() const
{
	mySplashWindow->Close();
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

#if UseScriptGraph
	const float delta = Timer::GetInstance().GetDeltaTime();
	ScriptEditor->Update(delta);
	ScriptEditor->Render();
#endif
}

void Editor::Update()
{
	OPTICK_EVENT();
	Timer::GetInstance().Update();
	const float delta = Timer::GetInstance().GetDeltaTime();


	Shipyard_PhysX::Get().StartRead();
	GameObjectManager::Get().Update();
	myGameLauncher.Update(delta);
	DebugDrawer::Get().Update(delta);

	//Shipyard_PhysX::Get().Render();
	Shipyard_PhysX::Get().EndRead(delta);
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
	return ViewportRect;
}

Vector2<int> Editor::GetViewportResolution()
{
	const RECT rect = GetViewportRECT();
	return Vector2<int>(
		rect.right - rect.left,
		rect.bottom - rect.top
	);
}

void Editor::ExpandWorldBounds(Sphere<float> sphere)
{
	if (myWorldBounds.ExpandSphere(sphere))
	{
		//MVLogger.Log("World bounds was expanded");
		for (auto& i : GameObjectManager::Get().GetAllComponents<cLight>())
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