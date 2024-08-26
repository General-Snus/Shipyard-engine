#define NOMINMAX
#include "../ContentDirectory.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Tools/ImGui/ImGui/ImGuiHelpers.hpp>
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

ContentDirectory::ContentDirectory() : m_CurrentPath(AssetManager::AssetPath)
{
    Editor::Get().m_Callbacks[EditorCallback::WM_DropFile].AddListener([this]() { this->WMDroppedFile(); });
}

void PopUpMenu()
{
}

void ContentDirectory::Node(const int index, const float cellWidth)
{
    const std::filesystem::path &fullPath = m_CurrentDirectoryPaths[index].second;

    const auto &path = std::filesystem::relative(fullPath, AssetManager::AssetPath);
    const auto &fileName = path.filename();
    const auto &extension = path.extension();

    const std::shared_ptr<TextureHolder> imageTexture = IconFromExtension(fullPath, extension, path);
    ImGui::ImageButton(fileName.string().c_str(), imageTexture, {cellWidth, cellWidth});
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        const std::string payload = path.string();
        ImGui::SetDragDropPayload(std::format("ContentAsset_{}", AssetManager::AssetType(path)).c_str(),
                                  payload.c_str(), payload.size());
        ImGui::Text(fullPath.stem().string().c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginPopupContextItem(path.string().c_str()))
    {
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
            PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(std::filesystem::absolute(fullPath).wstring().c_str());
            if (pidl)
            {
                SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
                ILFree(pidl);
            }
        }
        if (ImGui::Selectable("Open"))
        {
            if (auto asset = AssetManager::TryLoadAsset(path))
            {
                auto newWindow = std::make_shared<CustomFuncWindow>(std::bind(&AssetBase::InspectorView, asset));
                newWindow->SetWindowName(asset->GetAssetPath().filename().string());
                Editor::Get().g_EditorWindows.emplace_back(newWindow);
            }
        }
        if (ImGui::Selectable("Open External"))
        {
            ShellExecute(0, 0, fullPath.wstring().c_str(), 0, 0, SW_SHOW);
        }
        if (ImGui::Selectable("Delete"))
        {
            std::filesystem::remove(std::filesystem::absolute(fullPath));
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
            const char *output = std::filesystem::absolute(fullPath).string().c_str();
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
            if (auto asset = AssetManager::TryLoadAsset(path))
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
        m_hoveredPath = fullPath;
    }
    if (isHovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        if (std::filesystem::is_directory(fullPath))
        {
            m_CurrentPath = fullPath;

            ZeroMemory(buf, 128);
            IsDirty = true;
        }
        else
        {
            ShellExecute(0, 0, fullPath.wstring().c_str(), 0, 0, SW_SHOW);
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
        ImGui::SetNextItemWidth(150);
        IsDirty |= ImGui::InputText("Search", buf, IM_ARRAYSIZE(buf));
        DirectoryBar();

        ImGui::EndMenuBar();
    }
    const std::string keyTerm = buf;

    if (IsDirty)
    {
        FillPaths(keyTerm);
        IsDirty = false;
    }

    // Sort if searching
    if (!keyTerm.empty())
    {
        std::ranges::sort(m_CurrentDirectoryPaths, [=](const auto &a, const auto &b) {
            return Levenstein::Distance(a.first, keyTerm) < Levenstein::Distance(b.first, keyTerm);
        });
    }

    if (ImGui::IsWindowFocused() && Input::IsKeyHeld(Keys::CONTROL))
    {
        cellSize += Input::GetMouseWheelDelta() * Timer::GetDeltaTime();
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
        DoMoveOperation();
    }

    ImGui::End();
}

void ContentDirectory::FillPaths(const std::string &keyTerm)
{
    m_CurrentDirectoryPaths.clear();
    if (!keyTerm.empty())
    {
        for (const auto &it : std::filesystem::recursive_directory_iterator(AssetManager::AssetPath))
        {
            const std::string name = it.path().filename().string();
            m_CurrentDirectoryPaths.emplace_back(name, it.path());
        }
    }
    else
    {
        for (const auto &it : std::filesystem::directory_iterator(m_CurrentPath))
        {
            const std::string name = it.path().filename().string();
            m_CurrentDirectoryPaths.emplace_back(name, it.path());
        }
    }
}

void ContentDirectory::DoMoveOperation()
{
    if (isLookingForHovered && m_hoveredPath.empty())
    {
        m_hoveredPath = std::filesystem::absolute(m_CurrentPath);
        isLookingForHovered = false;
    }

    auto &paths = Editor::Get().WM_DroppedPath;

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
                Logger::Err(e.what());
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

std::shared_ptr<TextureHolder> ContentDirectory::IconFromExtension(const std::filesystem::path &fullPath,
                                                                   const std::filesystem::path &extension,
                                                                   const std::filesystem::path &path)
{
    std::shared_ptr<TextureHolder> imageTexture;
    if (std::filesystem::is_directory(fullPath))
    {
        imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/Folder.png");
    }

    else if (extension == ".dds" || extension == ".png" || extension == ".hdr" || extension == ".jpg")
    {
        imageTexture = AssetManager::Get().LoadAsset<TextureHolder>(path);
        if (!imageTexture->isLoadedComplete)
        {
            imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
        }
    }

    else if (extension == ".fbx")
    {
        std::shared_ptr<Mesh> mesh = AssetManager::Get().LoadAsset<Mesh>(path);
        if (mesh && mesh->isLoadedComplete)
        {
            imageTexture = mesh->GetEditorIcon();
        }
    }

    else if (extension == ".json")
    {
        std::shared_ptr<Material> materialPreview = AssetManager::Get().LoadAsset<Material>(path);
        if (materialPreview && materialPreview->isLoadedComplete)
        {
            imageTexture = materialPreview->GetEditorIcon();
        }
    }

    else if (extension == ".Graph")
    {
        imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/Graph.png");
    }

    else if (extension == ".cso")
    {
        imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
    }

    if (!imageTexture)
    {
        imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
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
    if (ImGui::ArrowButton("##BackButton", ImGuiDir_Up) && m_CurrentPath != AssetManager::AssetPath)
    {
        IsDirty = true;
        m_CurrentPath = m_CurrentPath.parent_path();
    }

    ImGui::SameLine();
    std::filesystem::path recursivePath = m_CurrentPath;
    if (ImGui::Button(AssetManager::AssetPath.string().c_str()))
    {
        IsDirty = true;
        m_CurrentPath = AssetManager::AssetPath;
    }

    ImGui::SameLine();
    std::vector<std::filesystem::path> pathList;
    while (recursivePath != AssetManager::AssetPath)
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
    auto path = Editor::Get().WM_DroppedPath;
    if (!path.empty())
    {
        isLookingForHovered = true;
        isMoveOperationUnderway = true;
    }
}
