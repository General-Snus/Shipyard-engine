#define NOMINMAX
#include "../ContentDirectory.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Tools/ImGui/ImGui/ImGuiHepers.hpp>
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
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>
#include <Font/IconsFontAwesome5.h>

ContentDirectory::ContentDirectory() : m_CurrentPath(AssetManager::AssetPath)
{
    Editor::Get().m_Callbacks[EditorCallback::WM_DropFile].AddListener([this]() { this->WMDroppedFile(); });
}

void ContentDirectory::RenderImGUi()
{
    OPTICK_EVENT();
    ImGui::Begin("ContentFolder", &m_KeepWindow);

    DirectoryBar();

    if (ImGui::IsWindowFocused() && Input::IsKeyHeld(Keys::CONTROL))
    {
        cellSize += Input::GetMouseWheelDelta() * Timer::GetDeltaTime();
        cellSize = std::clamp(cellSize, 50.f, 300.f);
    }

    ImGui::NewLine();
    const float contentFolderWidth = ImGui::GetContentRegionAvail().x;
    const auto cellWidth = cellSize;
    auto columnCount = static_cast<int>(contentFolderWidth / cellWidth);

    ImGui::Columns(std::max(columnCount, 1), nullptr);

    if (IsDirty)
    {
        m_CurrentDirectoryPaths.clear();
        for (const auto &it : std::filesystem::directory_iterator(m_CurrentPath))
        {
            m_CurrentDirectoryPaths.emplace_back(it.path());
        }
        IsDirty = false;
    }

    for (const auto &fullPath : m_CurrentDirectoryPaths)
    {
        OPTICK_EVENT("DirectoryIteration");
        const auto &path = std::filesystem::relative(fullPath, AssetManager::AssetPath);
        const auto &fileName = path.filename();
        const auto &extension = path.extension();

        std::shared_ptr<TextureHolder> imageTexture = IconFromExtension(fullPath, extension, path);

        ImGui::ImageButton(fileName.string().c_str(), imageTexture, {cellWidth, cellWidth});

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            const std::string payload = path.string();
            auto type = std::format("ContentAsset_{}", AssetManager::AssetType(path)).c_str();
            ImGui::SetDragDropPayload(type, payload.c_str(), payload.size());
            ImGui::Text(fullPath.stem().string().c_str());
            ImGui::EndDragDropSource();
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
                m_CurrentPath /= fileName;
                IsDirty = true;
            }
            else
            {
                ShellExecute(0, 0, fullPath.wstring().c_str(), 0, 0, SW_SHOW);
            }
        }
        ImGui::TextWrapped(fileName.string().c_str());
        ImGui::NextColumn();
    }

    if (isMoveOperationUnderway)
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
                ImGui::OpenPopup("Delete?");

                ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

                if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::PushID((void *)&pathToMoveFrom);
                    ImGui::Text(std::format("{} already exists here! Changes cannot be undone",
                                            pathToMoveFrom.filename().string())
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

                    ImGui::PopID();
                }
            }

            if (exists == overwrite)
            {
                if (!std::filesystem::is_directory(m_hoveredPath))
                {
                    m_hoveredPath = m_hoveredPath.parent_path();
                }
                try
                {
                    std::filesystem::copy(pathToMoveFrom, m_hoveredPath,
                                          std::filesystem::copy_options::overwrite_existing);
                }
                catch (std::filesystem::filesystem_error &e)
                {
                    Logger::Err(e.what());
                }
                paths.pop_back();
            }
        }
        IsDirty = true;
    }

    ImGui::End();
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

    else if (extension == ".dds" || extension == ".png" || extension == ".hdr")
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
