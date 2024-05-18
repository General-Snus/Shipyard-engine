#define NOMINMAX 

#include <assert.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <Windows.h>

#include <Editor/Editor/Windows/Window.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXDynamicBody.h> 
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h> 

#include <Tools/ImGui/ImGui/backends/imgui_impl_dx12.h>
#include <Tools/ImGUI/ImGUI/imgui.h>  
#include <Tools/ImGUI/ImGUI/imgui_impl_win32.h>
#include "ImGuizmo.h"


#include <Tools/Logging/Logging.h>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/System/ThreadPool.hpp>
#include "../Editor.h"
#include "AssetManager.h"
#include "ComponentSystem/Components/LightComponent.h"
#include "DirectX/Shipyard/GPU.h"
#include "GraphicsEngine.h"   
#include "System/SceneGraph/WorldGraph.h"
#include "Windows/EditorWindows/Console.h"
#include "Windows/EditorWindows/ContentDirectory.h"
#include "Windows/EditorWindows/Hierarchy.h"
#include "Windows/EditorWindows/Inspector.h"
#include "Windows/EditorWindows/Viewport.h"
#include "Windows/SplashWindow.h" 
#if PHYSX 
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#endif // PHYSX 0

using json = nlohmann::json;

bool Editor::Initialize(HWND aHandle)
{
	if (constexpr bool profileStartup = true) {
		OPTICK_START_CAPTURE();
	}
	Logger::Create();
	Logger::SetPrintToVSOutput(true);
	GetWindowRect(Window::windowHandler,&ViewportRect);
	ShowSplashScreen();
	ThreadPool::Get().Init();


#ifdef _DEBUG
	GraphicsEngine::Get().Initialize(aHandle,true);
#else
	GraphicsEngine::Get().Initialize(aHandle,false);
#endif // Release

	AddViewPort();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;



	ImFontConfig font_config{};
	const std::string font_path = AssetManager::AssetPath.string() + "Fonts/roboto/Roboto-Light.ttf";
	io.Fonts->AddFontFromFileTTF(font_path.c_str(),16.0f,&font_config);
	font_config.MergeMode = true;

#define FONT_ICON_FILE_NAME_FAR "fa-regular-400.ttf"
#define FONT_ICON_FILE_NAME_FAS "fa-solid-900.ttf" 
#define ICON_MIN_FA 0xe005
#define ICON_MAX_16_FA 0xf8ff
#define ICON_MAX_FA 0xf8ff

	ImWchar const icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	const std::string icon_path = AssetManager::AssetPath.string() + "Fonts/FontAwesome/" FONT_ICON_FILE_NAME_FAS;
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

	Input::UpdateEvents(aMessage.message,aMessage.wParam,aMessage.lParam);
	Input::UpdateMouseInput(aMessage.message);
}

int	 Editor::Run()
{
	OPTICK_FRAME("MainThread");
	Input::Update();

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
	OPTICK_EVENT();

	ImGui_ImplDX12_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplDX12_NewFrame",Optick::Category::UI);
	ImGui_ImplWin32_NewFrame();
	OPTICK_CATEGORY("ImGui_ImplWin32_NewFrame",Optick::Category::UI);
	ImGui::NewFrame();
	OPTICK_CATEGORY("ImGui::NewFrame",Optick::Category::UI);
	ImGuizmo::BeginFrame();

	ImGui::DockSpaceOverViewport();
	TopBar();

#if UseScriptGraph
	const float delta = Timer::GetInstance().GetDeltaTime();
	ScriptEditor->Update(delta);
	ScriptEditor->Render();
#endif
}

void Editor::Update()
{
	OPTICK_FRAME_EVENT(Optick::FrameType::CPU);
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
	OPTICK_EVENT();

	for (auto& viewport : m_Viewports)
	{
		viewport->Update();
	}
	GraphicsEngine::Get().Render(m_Viewports);/*

	const float delta = Timer::GetInstance().GetDeltaTime();
	const unsigned sleepTime = static_cast<unsigned>(((1.f / 60.f) * 1000.f));
	if (sleepTime > 0 && sleepTime < 17)*/
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}
}

void Editor::AddViewPort()
{
	static int ViewportIndex = 0;
	auto viewport = std::make_shared<Viewport>(!static_cast<bool>(ViewportIndex));
	viewport->ViewportIndex = ViewportIndex;
	ViewportIndex++;
	m_Viewports.emplace_back(viewport);
	g_EditorWindows.emplace_back(viewport);
}

void Editor::TopBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::Selectable("Open Scene"))
			{

			}
			if (ImGui::Selectable("New Scene"))
			{

			}
			if (ImGui::Selectable("Save Scene"))
			{

			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::Selectable("Viewport"))
			{
				AddViewPort();
			}

			if (ImGui::Selectable("Inspector"))
			{
				g_EditorWindows.emplace_back(std::make_shared<Inspector>());
			}

			if (ImGui::Selectable("Hierarchy"))
			{
				g_EditorWindows.emplace_back(std::make_shared<Hierarchy>());
			}

			if (ImGui::Selectable("Content Browser"))
			{
				g_EditorWindows.emplace_back(std::make_shared<ContentDirectory>());
			}

			if (ImGui::Selectable("Console"))
			{
				g_EditorWindows.emplace_back(std::make_shared<Console>());
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	for (const auto& windows : g_EditorWindows)
	{
		windows->RenderImGUi();
	}
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