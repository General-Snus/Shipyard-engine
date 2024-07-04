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
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

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
#include "DirectX/Shipyard/GPU.h"
#include "Editor/Editor/Windows/EditorWindows/Console.h"
#include "Editor/Editor/Windows/EditorWindows/ContentDirectory.h"
#include "Editor/Editor/Windows/EditorWindows/Hierarchy.h"
#include "Editor/Editor/Windows/EditorWindows/Inspector.h"
#include "Editor/Editor/Windows/EditorWindows/Viewport.h"
#include "Editor/Editor/Windows/SplashWindow.h" 
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/Components/LightComponent.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"   
#include "Engine/PersistentSystems/Scene.h"
#include "Engine/PersistentSystems/System/SceneGraph/WorldGraph.h"
#if PHYSX 
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#endif // PHYSX 0
#include <Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h>
#include <json.h>
#include <Tools/Utilities/Color.hpp>

void SetupImGuiStyle(bool light = false)
{
	OPTICK_EVENT();
	ImGuizmo::AllowAxisFlip(false);
	auto& guizmoStyle = ImGuizmo::GetStyle();


	guizmoStyle.TranslationLineThickness = 6.0f;
	guizmoStyle.TranslationLineArrowSize = 8.0f;
	guizmoStyle.RotationLineThickness = 4.0f;
	guizmoStyle.RotationOuterLineThickness = 5.0f;
	guizmoStyle.ScaleLineThickness = 5.0f;
	guizmoStyle.ScaleLineCircleSize = 7.0f;
	guizmoStyle.HatchedAxisLineThickness = 6.0f;
	guizmoStyle.CenterCircleSize = 8.0f;



	// Rounded Visual Studio style by RedNicStone from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(10.0f,10.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f,32.0f);
	style.WindowTitleAlign = ImVec2(0.0f,0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f,3.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f,4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f,4.0f);
	style.CellPadding = ImVec2(4.0f,2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 20.0f;
	style.ScrollbarRounding = 0.f;
	style.GrabMinSize = 20.0f;
	style.GrabRounding = 0.f;
	style.TabRounding = 0.f;
	style.TabBorderSize = 4.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f,0.5f);
	style.SelectableTextAlign = ImVec2(0.0f,0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f,1.0f,1.0f,1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434f,0.5921568870544434f,0.5921568870544434f,1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.3058823645114899f,0.3058823645114899f,0.3058823645114899f,1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899f,0.3058823645114899f,0.3058823645114899f,1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319f,0.321568638086319f,0.3333333432674408f,1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f,0.3529411852359772f,0.3725490272045135f,1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f,0.3529411852359772f,0.3725490272045135f,1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899f,0.3058823645114899f,0.3058823645114899f,1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3058823645114899f,0.3058823645114899f,0.3058823645114899f,1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899f,0.3058823645114899f,0.3058823645114899f,1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322f,0.2000000029802322f,0.2156862765550613f,1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319f,0.321568638086319f,0.3333333432674408f,1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f,0.5921568870544434f,0.9254902005195618f,1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f,0.1882352977991104f,0.2000000029802322f,1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f,0.3098039329051971f,0.3490196168422699f,1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f,0.2274509817361832f,0.2470588237047195f,1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f,0.0f,0.0f,0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f,1.0f,1.0f,0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f,0.4666666686534882f,0.7843137383460999f,1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f,1.0f,1.0f,0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f,0.800000011920929f,0.800000011920929f,0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f,0.1450980454683304f,0.1490196138620377f,1.0f);

	auto path = AssetManager::AssetPath / "Theme.json";
	if (std::filesystem::exists(path))
	{
		std::ifstream file(path);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();


		nlohmann::json js;
		if (light)
		{
			js = json["Light"];
		}
		else
		{
			js = json["Dark"];
		}


		try
		{
			auto primary = Color(js["PrimaryColor"]);
			auto secondary = Color(js["SecondaryColor"]);
			auto Detail = Color(js["DetailColor"]);
			auto text = Color(js["TextColor"]);
			auto active = Color(js["Active/Loaded"]);
			auto inactive = Color(js["Inactive/Unloaded"]);


			const float onClick = .9f;
			const float onHover = 1.1f;



			style.Colors[ImGuiCol_WindowBg] = primary.GetRGBA();
			style.Colors[ImGuiCol_TitleBgActive] = primary.GetRGBA();
			style.Colors[ImGuiCol_TitleBgCollapsed] = primary.GetRGBA();
			style.Colors[ImGuiCol_ChildBg] = primary.GetRGBA();
			style.Colors[ImGuiCol_PopupBg] = primary.GetRGBA();
			style.Colors[ImGuiCol_TitleBg] = primary.GetRGBA();
			style.Colors[ImGuiCol_MenuBarBg] = secondary.GetRGBA();
			style.Colors[ImGuiCol_Header] = secondary.GetRGBA();
			style.Colors[ImGuiCol_Tab] = secondary.GetRGBA();
			style.Colors[ImGuiCol_Border] = secondary.GetRGBA();

			style.Colors[ImGuiCol_Separator] = secondary.GetRGBA();
			style.Colors[ImGuiCol_SeparatorActive] = secondary.GetRGBA() * onClick;
			style.Colors[ImGuiCol_SeparatorHovered] = secondary.GetRGBA() * onHover;

			style.Colors[ImGuiCol_ResizeGrip] = Detail.GetRGBA();
			style.Colors[ImGuiCol_ResizeGripActive] = Detail.GetRGBA() * onClick;
			style.Colors[ImGuiCol_ResizeGripHovered] = Detail.GetRGBA() * onHover;

			style.Colors[ImGuiCol_Button] = secondary.GetRGBA();
			style.Colors[ImGuiCol_ButtonActive] = Detail.GetRGBA() * onClick;
			style.Colors[ImGuiCol_ButtonHovered] = Detail.GetRGBA() * onHover;

			style.Colors[ImGuiCol_ScrollbarBg] = secondary.GetRGBA();
			style.Colors[ImGuiCol_ScrollbarGrab] = Detail.GetRGBA();
			style.Colors[ImGuiCol_ScrollbarGrabActive] = Detail.GetRGBA() * onClick;
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = Detail.GetRGBA() * onHover;

			style.Colors[ImGuiCol_CheckMark] = Detail.GetRGBA();
			style.Colors[ImGuiCol_SliderGrab] = Detail.GetRGBA();
			style.Colors[ImGuiCol_SliderGrabActive] = Detail.GetRGBA() * onClick;

			style.Colors[ImGuiCol_FrameBg] = secondary.GetRGBA();
			style.Colors[ImGuiCol_FrameBgActive] = Detail.GetRGBA() * onClick;
			style.Colors[ImGuiCol_FrameBgHovered] = Detail.GetRGBA() * onHover;

			style.Colors[ImGuiCol_HeaderActive] = Detail.GetRGBA();
			style.Colors[ImGuiCol_HeaderHovered] = Detail.GetRGBA() * onHover;

			style.Colors[ImGuiCol_DockingPreview] = Detail.GetRGBA();

			style.Colors[ImGuiCol_TabActive] = Detail.GetRGBA();
			style.Colors[ImGuiCol_TabHovered] = Detail.GetRGBA() * onHover;
			style.Colors[ImGuiCol_TabUnfocused] = primary.GetRGBA();
			style.Colors[ImGuiCol_TabUnfocusedActive] = Detail.GetRGBA();
			style.Colors[ImGuiCol_TabSelected] = Detail.GetRGBA() * onClick;
			style.Colors[ImGuiCol_TabSelectedOverline] = Detail.GetRGBA();

			style.Colors[ImGuiCol_Text] = text.GetRGBA();
			style.Colors[ImGuiCol_TextSelectedBg] = Detail.GetRGBA();


			style.Colors[ImGuiCol_PlotLines] = Detail.GetRGBA();
			style.Colors[ImGuiCol_PlotLinesHovered] = Detail.GetRGBA() * onHover;
			style.Colors[ImGuiCol_PlotHistogram] = Detail.GetRGBA();
			style.Colors[ImGuiCol_PlotHistogramHovered] = Detail.GetRGBA() * onHover;



		}
		catch (const nlohmann::json::exception& e)
		{
			std::string msg = std::format("Unsuccessfull loading of theme file at path: Theme.json {} ",e.what());
			Logger::Warn(msg);
		}

	}
}
void LoadFont()
{
	bool haveLoadedFont = false;
	ImGuiIO& io = ImGui::GetIO();

	auto path = AssetManager::AssetPath / "Theme.json";
	if (std::filesystem::exists(path))
	{
		std::ifstream file(path);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		try
		{
			float fontSize = json["FontSize"];
			std::filesystem::path FontPath = json["FontPath"];

			ImFontConfig font_config{};
			const std::string backupFont = AssetManager::AssetPath.string() + "/Fonts/roboto/Roboto-Light.ttf";
			const std::string font_path = AssetManager::AssetPath.string() + (FontPath).string();
			if (!io.Fonts->AddFontFromFileTTF(font_path.c_str(),fontSize,&font_config))
			{
				io.Fonts->AddFontFromFileTTF(backupFont.c_str(),16.0f,&font_config);
			}
			font_config.MergeMode = true;

			if (!io.Fonts->Build())
			{
				Logger::Err("fucked up font load");
			}
			haveLoadedFont = true;
		}
		catch (const nlohmann::json::exception& e)
		{
			std::string msg = std::format("Unsuccessfull loading of theme file at path: Theme.json {} ",e.what());
			Logger::Warn(msg);
		}
	}

	if (!haveLoadedFont)
	{
		ImFontConfig font_config{};
		const std::string backupFont = AssetManager::AssetPath.string() + "/Fonts/roboto/Roboto-Light.ttf";
		io.Fonts->AddFontFromFileTTF(backupFont.c_str(),16.0f,&font_config);
		font_config.MergeMode = true;

		if (!io.Fonts->Build())
		{
			Logger::Err("fucked up font load");
		}
	}
}

bool Editor::Initialize(HWND aHandle)
{
	if (constexpr bool profileStartup = false) {
		OPTICK_START_CAPTURE();
	}
	OPTICK_EVENT();
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


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
	io.ConfigDockingWithShift = true;

	ImGui_ImplWin32_Init(aHandle);
	if (const auto& heap = GPU::m_ResourceDescriptors[(int)eHeapTypes::HEAP_TYPE_CBV_SRV_UAV];
		!ImGui_ImplDX12_Init(
			GPU::m_Device.Get(),
			GPU::m_FrameCount,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			heap->Heap(),
			heap->GetCpuHandle(2000),heap->GetGpuHandle(2000)))
	{
		Logger::Err("Failed to init IMGUI Dx12");
	}

	LoadFont();
	SetupImGuiStyle();

#if PHYSX
	Shipyard_PhysX::Get().InitializePhysx();
#endif // PHYSX 0


	m_MainScene = std::make_shared<Scene>();

	Scene::ActiveManager().SetUpdatePriority<Transform>(ComponentManagerBase::UpdatePriority::Transform);
	Scene::ActiveManager().SetUpdatePriority<cPhysics_Kinematic>(ComponentManagerBase::UpdatePriority::Physics);
	Scene::ActiveManager().SetUpdatePriority<cPhysXDynamicBody>(ComponentManagerBase::UpdatePriority::Physics);
	//Force no write to thread after this?
	WorldGraph::InitializeWorld();



	myGameLauncher.Init();
	myGameLauncher.Start();
	HideSplashScreen();

#if UseScriptGraph
	ScriptEditor = Graph::GraphTool::Get().GetScriptingEditor();
	ScriptEditor->Init();
#endif 

	m_Callbacks[EditorCallback::ObjectSelected] = Event();


	AddViewPort();
	g_EditorWindows.emplace_back(std::make_shared<Inspector>());
	g_EditorWindows.emplace_back(std::make_shared<Hierarchy>());
	g_EditorWindows.emplace_back(std::make_shared<ContentDirectory>());
	g_EditorWindows.emplace_back(std::make_shared<Console>());
	return true;
}

void Editor::DoWinProc(const MSG& aMessage)
{
	if (aMessage.message == WM_CLOSE)
	{
		ThreadPool::Get().Destroy();
		Shipyard_PhysX::Get().ShutdownPhysx();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		GPU::UnInitialize();
		return;
	}


	Input::UpdateEvents(aMessage.message,aMessage.wParam,aMessage.lParam);

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	if (ImGui_ImplWin32_WndProcHandler(aMessage.hwnd,aMessage.message,aMessage.wParam,aMessage.lParam))
	{
		return;
	}
}

int	 Editor::Run()
{
	OPTICK_FRAME("MainThread",Optick::FrameType::CPU);
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
	Input::Update();
	return 0;
}

void Editor::ShowSplashScreen()
{
	OPTICK_EVENT();
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
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
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
	OPTICK_EVENT();
	Timer::Update();
	const float delta = Timer::GetDeltaTime();

	Shipyard_PhysX::Get().StartRead();
	Scene::ActiveManager().Update();
	myGameLauncher.Update(delta);
	DebugDrawer::Get().Update(delta);
	//Shipyard_PhysX::Get().Render();
	Shipyard_PhysX::Get().EndRead(delta);
}

void Editor::Render()
{
	OPTICK_EVENT();
	std::vector<std::shared_ptr<Viewport>> test;
	for (auto& viewport : m_Viewports)
	{
		test.emplace_back(viewport);
		viewport->Update();
	}
	for (auto& viewport : m_CustomSceneRenderPasses)
	{
		test.emplace_back(viewport);
		viewport->Update();
	}



	GraphicsEngine::Get().Render(test);

	for (auto& viewport : m_CustomSceneRenderPasses)
	{
		viewport->m_RenderTarget->isBeingLoaded = false;
		viewport->m_RenderTarget->isLoadedComplete = true;
	}
	m_CustomSceneRenderPasses.clear();

	//{
	//	using namespace std::chrono_literals;
	//	std::this_thread::sleep_for(std::chrono::milliseconds(8));
	//}
}

void Editor::AddViewPort()
{
	OPTICK_EVENT();
	static int ViewportIndex = 0;
	auto viewport = std::make_shared<Viewport>(!static_cast<bool>(ViewportIndex));
	viewport->ViewportIndex = ViewportIndex;
	ViewportIndex++;
	m_Viewports.emplace_back(viewport);
	g_EditorWindows.emplace_back(viewport);
}

void Editor::TopBar()
{
	OPTICK_EVENT();
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
			if (ImGui::Selectable("ImGuiDemoWindow"))
			{
				g_EditorWindows.emplace_back(std::make_shared<CustomFuncWindow>(&ImGui::ShowDemoWindow,(bool*)0));
			}

			if (ImGui::Selectable("Light Theme"))
			{
				SetupImGuiStyle(true);
			}


			if (ImGui::Selectable("Dark Theme"))
			{
				SetupImGuiStyle(false);
			}


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

void Editor::AddRenderJob(std::shared_ptr<Viewport> aViewport)
{
	m_CustomSceneRenderPasses.emplace_back(aViewport);
}

uint32_t Editor::GetAmountOfRenderJob()
{
	return static_cast<uint32_t>(m_CustomSceneRenderPasses.size());
}
