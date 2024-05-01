#define NOMINMAX 

#include <assert.h>
#include <Editor/Editor/Windows/Window.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXDynamicBody.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXStaticBody.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h> 
#include <Engine/AssetManager/ComponentSystem/GameObject.h> 
#include <filesystem>
#include <fstream>
#include <functional>
#include <streambuf> 
#include <string>
#include <stringapiset.h>
#include <Tools/ImGui/ImGui/backends/imgui_impl_dx12.h>
#include <Tools/ImGUI/ImGUI/imgui.h>
#include <Tools/ImGUI/ImGUI/imgui_impl_dx11.h>
#include <Tools/ImGUI/ImGUI/imgui_impl_win32.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/InputHandler.hpp>
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/System/ThreadPool.hpp>
#include <Windows.h>
#include <Windows/EditorWindows/ChainGraph/GraphTool.h>
#include "../Editor.h"
#include "AssetManager.h"
#include "ComponentSystem/Components/LightComponent.h"
#include "Core/Paths.h"
#include "DirectX/Shipyard/GPU.h"
#include "GraphicsEngine.h" 
#include "System/SceneGraph/WorldGraph.h"
#include "Windows/SplashWindow.h"

#if PHYSX 
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#endif // PHYSX 0

using json = nlohmann::json;

bool Editor::Initialize(HWND aHandle)
{
	Logger::Create();
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
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch 
	// Setup Platform/Renderer backends

	ImFontConfig font_config{};
	std::string font_path = AssetManager::AssetPath.string() + "Fonts/roboto/Roboto-Light.ttf";
	io.Fonts->AddFontFromFileTTF(font_path.c_str(),16.0f,&font_config);
	font_config.MergeMode = true;

#define FONT_ICON_FILE_NAME_FAR "fa-regular-400.ttf"
#define FONT_ICON_FILE_NAME_FAS "fa-solid-900.ttf" 
#define ICON_MIN_FA 0xe005
#define ICON_MAX_16_FA 0xf8ff
#define ICON_MAX_FA 0xf8ff

	ImWchar const icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	std::string icon_path = AssetManager::AssetPath.string() + "Fonts/FontAwesome/" FONT_ICON_FILE_NAME_FAS;
	io.Fonts->AddFontFromFileTTF(icon_path.c_str(),15.0f,&font_config,icon_ranges);
	if (!io.Fonts->Build())
	{
		std::cout << "fucked up font load";
	}

	ImGui_ImplWin32_Init(aHandle);
	const D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = GPU::m_ImGui_Heap->GetFirstGpuHandle();
	const D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = GPU::m_ImGui_Heap->GetFirstCpuHandle();
	if (!ImGui_ImplDX12_Init(
		GPU::m_Device.Get(),
		GPU::m_FrameCount,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		GPU::m_ImGui_Heap->Heap(),
		cpu_handle,gpu_handle))
	{
		Logger::Err("Failed to init IMGUI Dx12");
	}

#if PHYSX
	Shipyard_PhysX::Get().InitializePhysx();
#endif // PHYSX 0


	GameObjectManager::Get().SetUpdatePriority<Transform>(ComponentManagerBase::UpdatePriority::Transform);
	GameObjectManager::Get().SetUpdatePriority<cPhysics_Kinematic>(ComponentManagerBase::UpdatePriority::Physics);
	GameObjectManager::Get().SetUpdatePriority<cPhysXDynamicBody>(ComponentManagerBase::UpdatePriority::Physics);
	//Force no write to thread after this?
	WorldGraph::InitializeWorld();



	myGameLauncher.Init();
	myGameLauncher.Start();
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
		GPU::UnInitialize();
		ImGui_ImplDX12_Shutdown();
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
	ImGui_ImplDX12_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplDX11_NewFrame",Optick::Category::UI);
	ImGui_ImplWin32_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplWin32_NewFrame",Optick::Category::UI);
	ImGui::NewFrame();
	OPTICK_CATEGORY("ImGui::NewFrame",Optick::Category::UI);
	ImGui::DockSpaceOverViewport();

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
	GraphicsEngine::Get().EndFrame();
}

void Editor::TopBar()
{
}

RECT Editor::GetViewportRECT()
{
	return ViewportRect;
}

Vector2<unsigned int> Editor::GetViewportResolution()
{
	const RECT rect = GetViewportRECT();
	return Vector2<unsigned int>(
		rect.right - rect.left,
		rect.bottom - rect.top
	);
}

void Editor::ExpandWorldBounds(const Sphere<float>& sphere)
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