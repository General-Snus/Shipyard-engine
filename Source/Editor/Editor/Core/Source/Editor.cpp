#define NOMINMAX

// TODO Un-yikes the includes
#include <assert.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <Windows.h>

#include <Editor/Editor/Windows/Window.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysXDynamicBody.h>

#include <Tools/ImGui/imgui.h>
#include <Tools/ImGui/backends/imgui_impl_dx12.h>
#include <Tools/ImGui/backends/imgui_impl_win32.h>
#include "ImGuizmo.h"

#include "../Editor.h"
#include <Tools/Logging/Logging.h>
#include <Tools/Optick/include/optick.h>
#include <Tools/Utilities/Color.h>
#include <Tools/Utilities/Game/Timer.h>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/System/ThreadPool.hpp>
#include "DirectX/DX12/Graphics/GPU.h"
#include "Editor/Editor/Windows/SplashWindow.h"
#include "Editor/Editor/Windows/EditorWindows/Console.h"
#include "Editor/Editor/Windows/EditorWindows/ContentDirectory.h"
#include "Editor/Editor/Windows/EditorWindows/Hierarchy.h"
#include "Editor/Editor/Windows/EditorWindows/Inspector.h"
#include "Editor/Editor/Windows/EditorWindows/Viewport.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/ComponentSystem/Components/LightComponent.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Engine/PersistentSystems/Scene.h"
#include "Engine/PersistentSystems/System/Colission/OnCollision.h"
#include "Engine/PersistentSystems/System/SceneGraph/WorldGraph.h"
#if PHYSX
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#endif // PHYSX 0
#include <CommCtrl.h>
#include <imgui_notify.h>
#include <stacktrace>
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Editor/Editor/Commands/SceneAction.h>
#include <Editor/Editor/Windows/EditorWindows/ChatWindow.h>
#include <Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h>
#include <Editor/Editor/Windows/EditorWindows/FrameStatistics.h>
#include <Editor/Editor/Windows/EditorWindows/GraphicsDebugger.h>
#include <Editor/Editor/Windows/EditorWindows/History.h>
#include <Tools/ImGui/Font/IconsFontAwesome6.h>
#include <Tools/ThirdParty/nlohmann/json.hpp>
#include <Tools/Utilities/System/ServiceLocator.h>
#include "Editor/Editor/Windows/EditorWindows/ColorPresets.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"

#include <shellapi.h>
#include <misc/cpp/WMDropManager.h>

enum Theme {
	light,
	dark,
	builtIn
};

void SetupImGuiStyle(Theme theme) {
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

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(10.0f,10.0f);
	style.WindowRounding = 2.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f,32.0f);
	style.WindowTitleAlign = ImVec2(0.0f,0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 2.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 2.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f,3.0f);
	style.FrameRounding = 2.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f,4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f,4.0f);
	style.CellPadding = ImVec2(4.0f,2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 20.0f;
	style.ScrollbarRounding = 2.f;
	style.GrabMinSize = 20.0f;
	style.GrabRounding = 2.f;
	style.TabRounding = 2.f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f,0.5f);
	style.SelectableTextAlign = ImVec2(0.0f,0.0f);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f,1.00f,1.00f,1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.59f,0.59f,0.59f,1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.24f,0.24f,0.24f,1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.24f,0.25f,0.27f,1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.31f,0.31f,0.31f,1.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.35f,0.35f,0.35f,1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f,1.00f,1.00f,0.16f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f,1.00f,1.00f,0.63f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.24f,0.24f,0.24f,1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f,0.25f,0.27f,0.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f,0.21f,0.21f,1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f,0.33f,0.33f,1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f,0.35f,0.35f,1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.73f,0.51f,0.30f,0.84f);
	colors[ImGuiCol_Button] = ImVec4(0.24f,0.25f,0.27f,0.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f,1.00f,1.00f,0.16f);
	colors[ImGuiCol_ButtonActive] = ImVec4(1.00f,1.00f,1.00f,0.63f);
	colors[ImGuiCol_Header] = ImVec4(0.29f,0.29f,0.29f,1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f,1.00f,1.00f,0.18f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f,1.00f,1.00f,0.63f);
	colors[ImGuiCol_Separator] = ImVec4(0.24f,0.25f,0.27f,1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f,1.00f,1.00f,0.16f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f,1.00f,1.00f,0.63f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.90f,0.62f,0.37f,1.10f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.73f,0.51f,0.30f,0.90f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.90f,0.62f,0.37f,1.10f);
	colors[ImGuiCol_Tab] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.90f,0.62f,0.37f,1.00f);
	colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.95f,0.51f,0.00f,0.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.14f,0.14f,0.14f,1.00f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.13f,0.13f,0.13f,1.00f);
	colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.82f,0.56f,0.33f,0.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f,0.20f,0.20f,1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f,0.62f,0.37f,1.10f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f,0.62f,0.37f,1.10f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f,0.19f,0.20f,1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f,0.31f,0.35f,1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f,0.23f,0.25f,1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f,0.00f,0.00f,0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f,1.00f,1.00f,0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.82f,0.56f,0.33f,1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.15f,0.15f,0.15f,1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.15f,0.15f,0.15f,1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.83f,0.41f,0.41f,0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f,0.80f,0.80f,0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.15f,0.15f,0.15f,1.00f);

	auto path = ENGINE_RESOURCES.Directory() / "Theme.json";
	if(theme != builtIn && exists(path)) {
		std::ifstream file(path);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		nlohmann::json js;
		if(theme == light) {
			js = json["Light"];
		} else {
			js = json["Dark"];
		}

		try {
			auto primary = Color(js["PrimaryColor"]);
			auto secondary = Color(js["SecondaryColor"]);
			auto Detail = Color(js["DetailColor"]);
			auto text = Color(js["TextColor"]);
			auto active = Color(js["Active/Loaded"]);
			auto inactive = Color(js["Inactive/Unloaded"]);

			constexpr float onClick = .9f;
			constexpr float onHover = 1.1f;

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
		} catch(const nlohmann::json::exception& e) {
			std::string msg = std::format("Unsuccessfull loading of theme file at path: Theme.json {} ",e.what());
			LOGGER.Warn(msg);
		}
	}
}

void LoadFont() {
	bool     haveLoadedFont = false;
	ImGuiIO& io = ImGui::GetIO();

	auto path = ENGINE_RESOURCES.Directory() / "Theme.json";
	if(exists(path)) {
		std::ifstream file(path);
		assert(file.is_open());

		nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		try {
			float                 fontSize = json["FontSize"];
			std::filesystem::path FontPath = json["FontPath"];

			const std::string backupFont = ENGINE_RESOURCES.Directory().string() + "/Fonts/roboto/Roboto-Light.ttf";
			const std::string awsomeFont = ENGINE_RESOURCES.Directory().string() +
				"/Fonts/FontAwesome/fa-solid-900.ttf";
			const std::string font_path = ENGINE_RESOURCES.Directory().string() + (FontPath).string();

			if(!io.Fonts->AddFontFromFileTTF(font_path.c_str(),fontSize)) {
				io.Fonts->AddFontFromFileTTF(backupFont.c_str(),16.0f);
			}

			static constexpr ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
			ImFontConfig             icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			icons_config.GlyphMinAdvanceX = fontSize * (2.f / 3.f);
			if(!io.Fonts->AddFontFromFileTTF(awsomeFont.c_str(),fontSize,&icons_config,icons_ranges)) {
				io.Fonts->AddFontFromFileTTF(backupFont.c_str(),16.0f);
			}

			if(!io.Fonts->Build()) {
				LOGGER.Err("fucked up font load");
			}
			haveLoadedFont = true;
		} catch(const nlohmann::json::exception& e) {
			std::string msg = std::format("Unsuccessfull loading of theme file at path: Theme.json {} ",e.what());
			LOGGER.Warn(msg);
		}
	}

	if(!haveLoadedFont) {
		ImFontConfig      font_config{};
		const std::string backupFont = ENGINE_RESOURCES.Directory().string() + "/Fonts/roboto/Roboto-Light.ttf";
		io.Fonts->AddFontFromFileTTF(backupFont.c_str(),16.0f,&font_config);
		font_config.MergeMode = true;

		if(!io.Fonts->Build()) {
			LOGGER.Err("fucked up font load");
		}
	}
	io.Fonts->AddFontDefault();
}

bool Editor::Initialize(HWND aHandle) {
	if(constexpr bool profileStartup = false) {
		OPTICK_START_CAPTURE();
	}
	OPTICK_EVENT();


	auto& timer = ServiceLocator::Instance().ProvideService<Timer>();
	auto& threadPool = ServiceLocator::Instance().ProvideService<ThreadPool>();

	auto& engineResources = ServiceLocator::Instance().ProvideService<EngineResourcesLoader>();
	engineResources.SetWorkingDirectory("../../Content");

	auto& colorManager = ServiceLocator::Instance().ProvideService<ColorManager>();
	auto& graphicsEngine = ServiceLocator::Instance().ProvideService<GraphicsEngine>();
	auto& physicsSystem = ServiceLocator::Instance().ProvideService<Shipyard_PhysX>();

	timer.initialize();
	threadPool.Init();

	ServiceLocator::Instance().ProvideService<LoggerService>();
	LOGGER.Create();
	LOGGER.SetPrintToVSOutput(true);

	GetWindowRect(WindowInstance.windowHandler,&ViewportRect);

	engineResources.RecursiveNameSave();
	colorManager.InitializeDefaultColors();
	colorManager.LoadColorsFromFile("Settings/ColorManagerData.ShipyardText");
#ifdef _DEBUG
	graphicsEngine.Initialize(true);
#else
	graphicsEngine.Initialize(true); // todo Disable
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
	if(const auto& heap = GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)];
		!ImGui_ImplDX12_Init(GPUInstance.m_Device.Get(),GPUInstance.m_FrameCount,DXGI_FORMAT_R8G8B8A8_UNORM,
			heap->Heap(),heap->GetCpuHandle(2000),heap->GetGpuHandle(2000))) {
		LOGGER.Err("Failed to init IMGUI Dx12");
	}

	LoadFont();
	SetupImGuiStyle(builtIn);

	// Set up imgui for multi context enviroment
	// This will have to be set in the init for every new context that is
	// loaded. You can acuire the data from the service locator
	auto& var = ServiceLocator::Instance().ProvideService<ImGui::ImGuiContextHolder>();
	var.ctx = ImGui::GetCurrentContext();
	ImGui::GetAllocatorFunctions(&var.v1,&var.v2,&var.v3);

#if PHYSX
	physicsSystem.InitializePhysx();
#endif // PHYSX 0

	m_ActiveScene = std::make_shared<Scene>("Editor Scene");

	Scene::activeManager().SetUpdatePriority<Transform>(ComponentManagerBase::UpdatePriority::Transform);
	Scene::activeManager().SetUpdatePriority<cPhysics_Kinematic>(ComponentManagerBase::UpdatePriority::Physics);
	Scene::activeManager().SetUpdatePriority<cPhysXDynamicBody>(ComponentManagerBase::UpdatePriority::Physics);
	// Force no write to thread after this?
	WorldGraph::InitializeWorld();

	m_Callbacks[EditorCallback::ObjectSelected] = Event();
	m_Callbacks[EditorCallback::SceneChange] = Event();
	m_Callbacks[EditorCallback::SceneChange].AddListener([]() { EDITOR_INSTANCE.GetSelectedGameObjects().clear(); });
	m_Callbacks[EditorCallback::WM_DropFile] = Event();
	gameState.Intialize("");

	AddViewPort();
	g_EditorWindows.reserve(100); // TODO Bruh
	g_EditorWindows.emplace_back(std::make_shared<Inspector>());
	g_EditorWindows.emplace_back(std::make_shared<Hierarchy>());
	g_EditorWindows.emplace_back(std::make_shared<ContentDirectory>());
	g_EditorWindows.emplace_back(std::make_shared<Console>());
	AddViewPort();
	return true;
}

void Editor::DoWinProc(const MSG& aMessage) {
	switch(aMessage.message) {
	case WM_DROPFILES:
	{
		const auto hDrop = reinterpret_cast<HDROP>(aMessage.wParam);
		const UINT numFiles = DragQueryFileW(hDrop,0xFFFFFFFF,nullptr,
			0); // Get the number of dropped files
		WM_DroppedPath.clear();
		for(UINT i = 0; i < numFiles; i++) {
			const UINT filePathLength = DragQueryFile(hDrop,i,nullptr,0); // Get the length of the file path

			std::wstring filePath;
			filePath.resize(filePathLength + 1); // Create a buffer to hold the file path
			DragQueryFile(hDrop,i,filePath.data(),
				filePathLength + 1); // Get the file path
			WM_DroppedPath.emplace_back(filePath);
		}

		DragFinish(hDrop); // Release the dropped files handle
		if(!WM_DroppedPath.empty()) {
			m_Callbacks[EditorCallback::WM_DropFile].Invoke();
		}
		break;
	}
	case WM_SIZE:
	{
		auto& graphicsEngine = ServiceLocator::Instance().GetService<GraphicsEngine>();
		const auto res = Vector2ui(LOWORD(aMessage.lParam),HIWORD(aMessage.lParam));
		graphicsEngine.ResizeBuffers(res);

		for(const auto& viewport : m_Viewports) {
			viewport->ResolutionUpdate();
		}
		break;
	}

	case WM_CLOSE:
		ColorManagerInstance.DumpToFile("Settings/ColorManagerData.ShipyardText");
		ThreadPoolInstance.Destroy();
		Shipyard_PhysXInstance.ShutdownPhysx();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		GPUInstance.UnInitialize();
		return;
	default:
		break;
	}

	Input.UpdateEvents(aMessage.message,aMessage.wParam,aMessage.lParam);

	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	if(ImGui_ImplWin32_WndProcHandler(aMessage.hwnd,aMessage.message,aMessage.wParam,aMessage.lParam)) {
	}
}

int Editor::Run() {
	OPTICK_FRAME("MainThread");
	if(IsGUIActive) {
		UpdateImGui();
		Update();
		Render();
	} else {
		Update();
		Render();
	}
	Input.Update();
	return 0;
}

void Editor::UpdateImGui() {
	OPTICK_EVENT();
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::DockSpaceOverViewport();
	TopBar();

	if(Input.IsKeyHeld(Keys::CONTROL) && Input.IsKeyPressed(Keys::Z)) {
		if(Input.IsKeyHeld(Keys::SHIFT)) {
			CommandBuffer::mainEditorCommandBuffer().redo();
		} else {
			CommandBuffer::mainEditorCommandBuffer().undo();
		}
	}
}

void Editor::CheckSelectedForRemoved() {
	OPTICK_EVENT();
	for(auto& selected : m_SelectedGameObjects) {
		if(!selected.scene().GetGOM().HasGameObject(selected.GetID())) {
			// TODO do other than just clear, clear only the value that is removed
			m_SelectedGameObjects.clear();
			break;
		}
	}
}

void Editor::Copy() {
	LOGGER.Success(std::format("Copied %i objects",GetSelectedGameObjects().size()),true);
}

void Editor::Paste() {
	auto          selected = GetSelectedGameObjects();
	CommandPacket packet;
	for(auto& object : copiedObjects) {
		auto components = object.CopyAllComponents();
		auto newObject = GameObject::Create(object.GetName() + "_1");
		for(const auto& i : components) {
			newObject.AddBaseComponent(i);
		}

		packet.emplace_back(std::make_shared<GameobjectAdded>(newObject));
	}

	CommandBuffer::mainEditorCommandBuffer().addCommand(packet);
	if(const auto ptr = CommandBuffer::mainEditorCommandBuffer().getLastCommand()) {
		ptr->setMergeBlocker(true);
	}
}

void Editor::FocusObject(const GameObject& focus,bool focusWithOffset) const {
	for(auto& viewport : m_Viewports) {
		if(!viewport->IsMainViewport()) {
			float radiusOffset = 0;
			if(const auto renderer = focus.TryGetComponent<MeshRenderer>()) {
				radiusOffset = renderer->GetBoundingBox().GetRadius();
			}

			const Vector3f position = focus.transform().GetPosition(WORLD);

			Transform& ref = viewport->GetCameraTransform();
			const Vector3f cameraPosition = ref.GetPosition(WORLD);
			const Vector3f direction = (position - cameraPosition).GetNormalized();

			if(!focusWithOffset) {
				radiusOffset = 0;
			}
			const Vector3f offset = -direction * radiusOffset;
			const Vector3f newPosition = position + offset;
			ref.LookAt(position);
			ref.SetPosition(newPosition);
		}
	}
}

void Editor::AlignObject(const GameObject& focus) const {
	for(auto& viewport : m_Viewports) {
		if(!viewport->IsMainViewport()) {
			Transform& ref = viewport->GetCameraTransform();
			const Vector3f position = ref.GetPosition(WORLD);
			const Vector3f rotation = ref.GetRotation();

			focus.transform().SetRotation(rotation);
			focus.transform().SetPosition(position);
		}
	}
}

void Editor::SetActiveScene(const std::shared_ptr<Scene> scene) {
	if(!scene) {
		LOGGER.Critical("Failed to set active scene: Scene was null or invalid");
		return;
	}

	m_Callbacks[EditorCallback::SceneChange].Invoke();
	m_ActiveScene = scene;
}

std::shared_ptr<Scene> Editor::GetActiveScene() {
	return m_ActiveScene;
}

std::shared_ptr<Viewport> Editor::GetMainViewport() {
	for(auto& view : m_Viewports) {
		if(view->IsMainViewport()) {
			return view;
		}
	}
	return nullptr;
}

void Editor::Update() {
	OPTICK_EVENT();
	TimerInstance.update();
	const float delta = TimerInstance.getDeltaTime();

	Shipyard_PhysXInstance.StartRead();
	Scene::activeManager().Update();
	SystemCollection::UpdateSystems(delta);

	// Editor key checks
	if(Input.IsKeyPressed(Keys::F) && m_SelectedGameObjects.size() > 0) {
		FocusObject(m_SelectedGameObjects[0]);
	}

	if(Input.IsKeyHeld(Keys::CONTROL) && Input.IsKeyPressed(Keys::C)) {
		Copy();
	}
	if(Input.IsKeyHeld(Keys::CONTROL) && Input.IsKeyPressed(Keys::V)) {
		Paste();
	}

	if(Input.IsKeyPressed(Keys::F10)) {
		ENGINE_RESOURCES.ClearUnused();
	}

	// End

	gameState.Update(delta);
	GraphicsEngineInstance.Update(delta);
	Shipyard_PhysXInstance.EndRead(delta);
}

void Editor::Render() {
	OPTICK_EVENT();
	for(const auto& viewport : m_Viewports) {
		viewport->Update();
	}
	GraphicsEngineInstance.Render(m_Viewports);
}

void Editor::AddViewPort() {
	OPTICK_EVENT();
	static int ViewportIndex = 0;
	auto       viewport = std::make_shared<Viewport>(!static_cast<bool>(ViewportIndex));
	viewport->ViewportIndex = ViewportIndex;
	ViewportIndex++;
	m_Viewports.emplace_back(viewport);
	g_EditorWindows.emplace_back(viewport);
}

void Editor::TopBar() {
	OPTICK_EVENT();
	if(ImGui::BeginMainMenuBar()) {
		if(ImGui::BeginMenu("File")) {
			if(ImGui::Selectable("Open Scene")) {
				// Here we can open the binary scene file
			}
			if(ImGui::Selectable("New Scene")) {
				// Here we can open the binary scene file
			}
			if(ImGui::Selectable("Save Scene")) {
				// Here we can open the binary scene file
			}

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit")) {
			if(ImGui::Selectable("ImGuiDemoWindow")) {
				auto window = std::make_shared<CustomFuncWindow>(&ImGui::ShowDemoWindow,static_cast<bool*>(nullptr));
				window->SetWindowName("ImGui demo holder");
				g_EditorWindows.emplace_back(window);
			}

			if(ImGui::Selectable("Light Theme")) {
				SetupImGuiStyle(light);
			}

			if(ImGui::Selectable("Dark Theme")) {
				SetupImGuiStyle(dark);
			}

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Window")) {
			if(ImGui::Selectable("Viewport")) {
				AddViewPort();
			}

			if(ImGui::Selectable("Inspector")) {
				g_EditorWindows.emplace_back(std::make_shared<Inspector>());
			}

			if(ImGui::Selectable("Hierarchy")) {
				g_EditorWindows.emplace_back(std::make_shared<Hierarchy>());
			}

			if(ImGui::Selectable("Content Browser")) {
				g_EditorWindows.emplace_back(std::make_shared<ContentDirectory>());
			}

			if(ImGui::Selectable("Console")) {
				g_EditorWindows.emplace_back(std::make_shared<Console>());
			}

			if(ImGui::Selectable("History")) {
				g_EditorWindows.emplace_back(std::make_shared<History>());
			}

			if(ImGui::Selectable("Color Presets")) {
				g_EditorWindows.emplace_back(std::make_shared<ColorPresets>());
			}

			if(ImGui::Selectable("Chat")) {
				g_EditorWindows.emplace_back(std::make_shared<ChatWindow>());
			}

			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Tools")) {
			if(ImGui::Selectable("Graphics debugger")) {
				g_EditorWindows.emplace_back(std::make_shared<GraphicsDebugger>());
			}

			if(ImGui::Selectable("Frame statistics")) {
				g_EditorWindows.emplace_back(std::make_shared<FrameStatistics>());
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if(ImGui::BeginMainMenuBar(1)) {
		if(gameState.IsPlaying && !gameState.IsPaused || gameState.IsLoading) {
			if(ImGui::Button(ICON_FA_PAUSE)) {
				gameState.PausePlaySession();
			}
		} else {
			if(ImGui::Button(ICON_FA_PLAY)) {
				gameState.StartPlaySession();
			}
		}

		if(ImGui::Button(ICON_FA_STOP)) {
			gameState.EndPlaySession();
		}

		ImGui::EndMainMenuBar();
	}
	for(const auto& windows : g_EditorWindows) {
		if(windows && windows->m_KeepWindow) {
			ImGui::PushID(windows.get());
			windows->RenderImGUi();
			ImGui::PopID();
		}
	}

	const auto [first,last] =
		std::ranges::remove_if(g_EditorWindows.begin(),g_EditorWindows.end(),
			[](const std::shared_ptr<EditorWindow>& window) { return !window->m_KeepWindow; });

	g_EditorWindows.erase(first,last);
}

RECT Editor::GetViewportRECT() {
	return ViewportRect;
}

Vector2<unsigned int> Editor::GetViewportResolution() {
	const RECT rect = GetViewportRECT();
	return Vector2<unsigned int>(rect.right - rect.left,rect.bottom - rect.top);
}
