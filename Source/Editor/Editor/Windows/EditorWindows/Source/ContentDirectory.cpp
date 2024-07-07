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
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>	
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h> 

#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Engine/PersistentSystems/Scene.h"
#include <Engine/GraphicsEngine/GraphicsEngineUtilities.h>



ContentDirectory::ContentDirectory() : m_CurrentPath(AssetManager::AssetPath)
{
	
}


void ContentDirectory::RenderImGUi()
{
	OPTICK_EVENT(); 
	ImGui::Begin("ContentFolder",&m_KeepWindow);

	{
		if (ImGui::ArrowButton("##BackButton",ImGuiDir_Up) && m_CurrentPath != AssetManager::AssetPath)
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
		for (const auto& path : std::views::reverse(pathList))
		{
			ImGui::SameLine();
			if (ImGui::Button(path.filename().string().c_str()))
			{
				IsDirty = true;
				m_CurrentPath = path;
			}
		}
	}

	if (ImGui::IsWindowFocused() && Input::IsKeyHeld(Keys::CONTROL))
	{
		cellSize += Input::GetMouseWheelDelta() * Timer::GetDeltaTime();
		cellSize = std::clamp(cellSize,50.f,300.f);
	}

	ImGui::NewLine();
	const float contentFolderWidth = ImGui::GetContentRegionAvail().x;
	const auto cellWidth = cellSize;
	auto columnCount = static_cast<int>(contentFolderWidth / cellWidth);


	ImGui::Columns(std::max(columnCount,1),nullptr);

	if (IsDirty)
	{
		m_CurrentDirectoryPaths.clear();
		for (const auto& it : std::filesystem::directory_iterator(m_CurrentPath))
		{
			m_CurrentDirectoryPaths.emplace_back(it.path());
		}
		IsDirty = false;
	}

	for (const auto& fullPath : m_CurrentDirectoryPaths)
	{
		const auto& path = std::filesystem::relative(fullPath,AssetManager::AssetPath);
		OPTICK_EVENT("DirectoryIteration");
		const auto& fileName = path.filename();
		const auto& extension = path.extension();

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
			imageTexture = AssetManager::Get().LoadAsset<TextureHolder>(std::format("INTERNAL_IMAGE_UI_{}",fileName.string()));
			std::shared_ptr<Mesh> mesh = AssetManager::Get().LoadAsset<Mesh>(path);

			if (!imageTexture->isLoadedComplete)
			{
				const bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
				if (!imageTexture->isBeingLoaded && meshReady)
				{
					GraphicsEngineUtilities::GenerateSceneForIcon(mesh,imageTexture,GraphicsEngine::Get().GetDefaultMaterial());
				}
				else
				{
					imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
				}
			}
		}

		else if (extension == ".json")
		{
			imageTexture = AssetManager::Get().LoadAsset<TextureHolder>(std::format("INTERNAL_IMAGE_UI_{}",fileName.string()));
			if (!imageTexture->isLoadedComplete)
			{
				std::shared_ptr<Mesh> mesh = AssetManager::Get().LoadAsset<Mesh>("Materials/MaterialPreviewMesh.fbx");
				std::shared_ptr<Material> materialPreview = AssetManager::Get().LoadAsset<Material>(path);

				bool meshReady = mesh->isLoadedComplete && !mesh->isBeingLoaded;
				bool materialReady = materialPreview->isLoadedComplete && !materialPreview->isBeingLoaded;


				if (!imageTexture->isBeingLoaded && meshReady && materialReady)
				{
					GraphicsEngineUtilities::GenerateSceneForIcon(mesh,imageTexture,materialPreview);
					Logger::Log("Material Preview Queued up");
				}
				else
				{
					imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
				}
			}
		}

		else if (extension == ".cso")
		{
			imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
		}

		else
		{
			imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
		}

		ImGui::ImageButton(fileName.string().c_str(),imageTexture,{ cellWidth,cellWidth });

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const std::string payload = path.string();
			auto type = std::format("ContentAsset_{}",AssetManager::AssetType(path)).c_str();
			ImGui::SetDragDropPayload(type,payload.c_str(),payload.size());
			ImGui::Text(fullPath.stem().string().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (std::filesystem::is_directory(fullPath))
			{
				m_CurrentPath /= fileName;
				IsDirty = true;
			}
			else
			{
				ShellExecute(0,0,fullPath.wstring().c_str(),0,0,SW_SHOW);
			}
		}
		ImGui::TextWrapped(fileName.string().c_str());
		ImGui::NextColumn();
	}

	ImGui::End();
}
