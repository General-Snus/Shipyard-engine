#define NOMINMAX
#include "../ContentDirectory.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Tools/ImGui/ImGuiHelpers.hpp>
#include <Tools/Utilities/Input/Input.hpp>
#include <shellapi.h>

#include <Editor/Editor/Core/Editor.h>
#include <Editor/Editor/Windows/EditorWindows/Viewport.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Engine/PersistentSystems/Scene.h"
#include <Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h>
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include <Font/IconsFontAwesome5.h>
#include <ShlObj.h>
#include <Tools/Utilities/Game/Timer.h>

ContentDirectory::ContentDirectory() : m_CurrentPath(EngineResources.Directory())
{
    EditorInstance.m_Callbacks[EditorCallback::WM_DropFile].AddListener([this]() { this->WMDroppedFile(); });
}

void PopUpMenu()
{
}

void ContentDirectory::Node(const int index, const float cellWidth)
{
	OPTICK_EVENT();
    const pathData &data = m_CurrentDirectoryPaths[index];
     
	const auto &path = data.fullPath;
	const auto &fileName = data.fullPath.filename();
	const auto &extension = path.extension();

    const std::shared_ptr<TextureHolder> imageTexture = IconFromExtension(extension, path, data.isDirectory);
	ImGui::ImageButton(fileName.string().c_str(), imageTexture, {cellWidth, cellWidth});
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		OPTICK_EVENT("BeginDragDropSource");
        const std::string payload = path.string();
        ImGui::SetDragDropPayload(std::format("ContentAsset_{}", EngineResources.AssetType(path)).c_str(),
                                  payload.c_str(), payload.size());
		ImGui::Text(path.stem().string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginPopupContextItem(path.string().c_str()))
	{
		OPTICK_EVENT("BeginPopupContextItem");
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::Selectable("Material Asset"))
            {
                IsDirty = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::Selectable("Show in Explorer"))
        {
			PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(
				std::filesystem::absolute(EngineResources.Directory() / path).wstring().c_str());
            if (pidl)
            {
                SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
                ILFree(pidl);
            }
        }
        if (ImGui::Selectable("Open"))
        {
          if (auto asset = EngineResources.TryLoadAsset(path))
            {
                auto newWindow = std::make_shared<CustomFuncWindow>(std::bind(&AssetBase::InspectorView, asset));
                newWindow->SetWindowName(asset->GetAssetPath().filename().string());
                EditorInstance.g_EditorWindows.emplace_back(newWindow);
            }
        }
        if (ImGui::Selectable("Open External"))
        {
			ShellExecute(0, 0, (EngineResources.Directory() / path).wstring().c_str(), 0, 0, SW_SHOW);
        }
        if (ImGui::Selectable("Delete"))
        {
			std::filesystem::remove(std::filesystem::absolute((EngineResources.Directory() / path)));
            IsDirty = true;
        }
        ImGui::Separator();
        if (ImGui::Selectable("Rename"))
        {
            IsDirty = true;
        }
        ImGui::Separator();
        if (ImGui::Selectable("Copy Path"))
        {
			const char *output = std::filesystem::absolute((EngineResources.Directory() / path)).string().c_str();
            const size_t len = strlen(output) + 1;
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
            memcpy(GlobalLock(hMem), output, len);
            GlobalUnlock(hMem);
            OpenClipboard(0);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
        }
        ImGui::Separator();
        if (ImGui::Selectable("ReImport"))
        {
          if (auto asset = EngineResources.TryLoadAsset(path))
            {
                asset->Init();
            }
            IsDirty = true;
        }
        ImGui::Separator();
        ImGui::EndPopup();
    }

    bool isHovered = ImGui::IsItemHovered();

    if (isHovered && isLookingForHovered)
    {
        isLookingForHovered = false;
		m_hoveredPath = (EngineResources.Directory() / path);
    }
    if (isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	{
		OPTICK_EVENT("IsMouseDoubleClicked");
        if (data.isDirectory)
        {
			m_CurrentPath = (EngineResources.Directory() / path);

            ZeroMemory(buf, 128);
            IsDirty = true;
        }
        else
        {
			ShellExecute(0, 0, (EngineResources.Directory() / path).wstring().c_str(), 0, 0, SW_SHOW);
        }
    }
    ImGui::Text(fileName.string().c_str());
}

void ContentDirectory::RenderImGUi()
{
    OPTICK_EVENT(); 
    ImGui::Begin("ContentFolder", &m_KeepWindow, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
	{
		OPTICK_EVENT("BeginMenuBar");
		ImGui::Spacing();
        ImGui::SetNextItemWidth(150);
        IsDirty |= ImGui::InputText("Search", buf, IM_ARRAYSIZE(buf));
        DirectoryBar();
		ImGui::Spacing();

        ImGui::EndMenuBar();
    }
    const std::string keyTerm = buf;

    if (IsDirty)
	{
		OPTICK_EVENT("fillPath");

        FillPaths(keyTerm);
        IsDirty = false;
    }

    // Sort if searching
    if (!keyTerm.empty())
    {
		OPTICK_EVENT("sorting");
        std::ranges::sort(m_CurrentDirectoryPaths, [=](const auto &a, const auto &b) {
			return Levenstein::Distance(a.name, keyTerm) < Levenstein::Distance(b.name, keyTerm);
        });
    }

    if (ImGui::IsWindowFocused() && Input.IsKeyHeld(Keys::CONTROL))
    {
        cellSize += Input.GetMouseWheelDelta() * TimerInstance.GetDeltaTime();
        cellSize = std::clamp(cellSize, 50.f, 300.f);
    }
    auto const &style = ImGui::GetStyle();

    const float cellWidth = cellSize + style.ItemInnerSpacing.x * 2.0f;
    const float cellHeight = cellSize + style.FramePadding.y * 2.0f + ImGui::CalcTextSize("asd\nasd").y;

    const float contentFolderWidth = ImGui::GetContentRegionAvail().x;
    const auto columnCount = std::max(static_cast<int>(contentFolderWidth / cellWidth), 1);
    const int clipperSize = std::max((int)std::round((float)m_CurrentDirectoryPaths.size() / columnCount), 1);

    if (ImGui::BeginTable("Table", columnCount))
	{
		OPTICK_EVENT("BeginTable");
        ImGuiListClipper clipper;
        clipper.Begin(clipperSize, cellHeight);
        while (clipper.Step())
        {
            OPTICK_EVENT("DirectoryIteration");
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                ImGui::TableNextRow();
                for (int collumn = 0; collumn < columnCount; collumn++)
				{
					OPTICK_EVENT("TableNextColumn");
                    ImGui::TableNextColumn();
                    const int index = row * columnCount + collumn;
                    if (index >= m_CurrentDirectoryPaths.size())
                    {
                        break;
                    }

                    Node(index, cellWidth);
                }
            }
        }
        ImGui::EndTable();
    }

    if (isMoveOperationUnderway)
	{
		OPTICK_EVENT("DoMoveOperation");
        DoMoveOperation();
    }

    ImGui::End();
}

void ContentDirectory::FillPaths(const std::string &keyTerm)
{
	OPTICK_EVENT();
    m_CurrentDirectoryPaths.clear();
    if (!keyTerm.empty())
    {
        for (const auto &it : std::filesystem::recursive_directory_iterator(EngineResources.Directory()))
		{
			OPTICK_EVENT("Iteration");
			const std::string name = it.path().filename().string();
			const auto relativePath = std::filesystem::relative(it.path(), EngineResources.Directory());

			std::filesystem::is_directory(it.path()) ? m_CurrentDirectoryPaths.emplace_back(name, relativePath, true)
													 : m_CurrentDirectoryPaths.emplace_back(name, relativePath, false); 
        }
    }
    else
    {
        for (const auto &it : std::filesystem::directory_iterator(m_CurrentPath))
		{
			OPTICK_EVENT("Iteration");
			const std::string name = it.path().filename().string();
			const auto relativePath = std::filesystem::relative(it.path(), EngineResources.Directory());

			std::filesystem::is_directory(it.path()) ? m_CurrentDirectoryPaths.emplace_back(name, relativePath, true)
													 : m_CurrentDirectoryPaths.emplace_back(name, relativePath, false); 
        }
    }
}

void ContentDirectory::DoMoveOperation()
{
	OPTICK_EVENT();
    if (isLookingForHovered && m_hoveredPath.empty())
    {
        m_hoveredPath = std::filesystem::absolute(m_CurrentPath);
        isLookingForHovered = false;
    }

    auto &paths = EditorInstance.WM_DroppedPath;

    for (auto &pathToMoveFrom : paths | std::ranges::views::reverse)
    {
        bool overwrite = false;
        bool exists = std::filesystem::exists(m_hoveredPath / pathToMoveFrom.filename());
        if (exists)
        {
            ImGui::PushID((void *)&pathToMoveFrom);
            const char *id = std::format("Delete{}", pathToMoveFrom.string()).c_str();
            ImGui::OpenPopup(id);

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal(id, NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text(
                    std::format("{} already exists here! Changes cannot be undone", pathToMoveFrom.filename().string())
                        .c_str());
                ImGui::Separator();

                if (ImGui::Button("Overwrite", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    overwrite = true;
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                    overwrite = false;
                }

                ImGui::EndPopup();
            }
            ImGui::PopID();
        }

        if (exists == overwrite)
        {
            if (!std::filesystem::is_directory(m_hoveredPath))
            {
                m_hoveredPath = m_hoveredPath.parent_path();
            }
            try
            {
                std::filesystem::copy(pathToMoveFrom, m_hoveredPath, std::filesystem::copy_options::overwrite_existing);
            }
            catch (std::filesystem::filesystem_error &e)
            {
                Logger.Err(e.what());
            }
            paths.pop_back();
        }

        if (paths.empty())
        {
            isLookingForHovered = false;
            isMoveOperationUnderway = false;
            m_hoveredPath = "";
        }
    }
    IsDirty = true;
}

std::shared_ptr<TextureHolder> ContentDirectory::IconFromExtension(const std::filesystem::path &extension,
                                                                   const std::filesystem::path &path, bool isDirectory)
{
	OPTICK_EVENT();
    std::shared_ptr<TextureHolder> imageTexture;
	if (isDirectory)
    {
        imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/Folder.png");
    }

    else if (extension == ".dds" || extension == ".png" || extension == ".hdr" || extension == ".jpg")
    {
        imageTexture = EngineResources.LoadAsset<TextureHolder>(path);
        if (!imageTexture->isLoadedComplete)
        {
            imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
        }
    }

    else if (extension == ".fbx")
    {
        std::shared_ptr<Mesh> mesh = EngineResources.LoadAsset<Mesh>(path);
        if (mesh && mesh->isLoadedComplete)
        {
            imageTexture = mesh->GetEditorIcon();
        }
    }

    else if (extension == ".json")
    {
        std::shared_ptr<Material> materialPreview = EngineResources.LoadAsset<Material>(path);
        if (materialPreview && materialPreview->isLoadedComplete)
        {
            imageTexture = materialPreview->GetEditorIcon();
        }
    }

    else if (extension == ".Graph")
    {
        imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/Graph.png");
    }

    else if (extension == ".cso" || extension == ".hlsl")
    {
        imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
    }

    if (!imageTexture)
    {
        imageTexture = EngineResources.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
    }

    return imageTexture;
}

void ContentDirectory::DirectoryBar()
{
    if (ImGui::Button(ICON_FA_SYNC))
    {
        IsDirty = true;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##BackButton", ImGuiDir_Up) && m_CurrentPath != EngineResources.Directory())
    {
        IsDirty = true;
        m_CurrentPath = m_CurrentPath.parent_path();
    }

    ImGui::SameLine();
    std::filesystem::path recursivePath = m_CurrentPath;
    if (ImGui::Button(EngineResources.Directory().string().c_str()))
    {
        IsDirty = true;
        m_CurrentPath = EngineResources.Directory();
    }

    ImGui::SameLine();
    std::vector<std::filesystem::path> pathList;
    while (recursivePath != EngineResources.Directory())
    {
        pathList.emplace_back(recursivePath);
        recursivePath = recursivePath.parent_path();
    }

    for (const auto &path : std::views::reverse(pathList))
    {
        ImGui::SameLine();
        if (ImGui::Button(path.filename().string().c_str()))
        {
            IsDirty = true;
            m_CurrentPath = path;
        }
    }
}

void ContentDirectory::WMDroppedFile()
{
    auto path = EditorInstance.WM_DroppedPath;
    if (!path.empty())
    {
        isLookingForHovered = true;
        isMoveOperationUnderway = true;
    }
}
