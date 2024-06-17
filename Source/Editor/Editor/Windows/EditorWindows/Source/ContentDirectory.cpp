#define NOMINMAX
#include "../ContentDirectory.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/GraphicsEngine/GraphicsEngine.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Tools/ImGui/ImGui/ImGuiHepers.hpp>
#include <Tools/Utilities/Input/Input.hpp>
#include <Tools/Utilities/Game/Timer.h>
#include <cctype>
#include <shellapi.h>
#pragma optimize("",off)

ContentDirectory::ContentDirectory() : m_CurrentPath(AssetManager::AssetPath)
{
}

void ContentDirectory::RenderImGUi()
{
	ImGui::Begin("ContentFolder");

	{
		if (ImGui::ArrowButton("##BackButton",ImGuiDir_Up) && m_CurrentPath != AssetManager::AssetPath)
		{
			m_CurrentPath = m_CurrentPath.parent_path();
		}
		ImGui::SameLine();
		std::filesystem::path recursivePath = m_CurrentPath;
		if (ImGui::Button(AssetManager::AssetPath.string().c_str()))
		{
			m_CurrentPath = AssetManager::AssetPath;
		}
		ImGui::SameLine();
		while (recursivePath != AssetManager::AssetPath)
		{
			ImGui::SameLine();
			if (ImGui::Button(recursivePath.filename().string().c_str()))
			{
				m_CurrentPath = recursivePath;
			}
			recursivePath = recursivePath.parent_path();
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
	for (const auto& it : std::filesystem::directory_iterator(m_CurrentPath))
	{
		const auto& path = std::filesystem::relative(it.path(),AssetManager::AssetPath);
		const auto& fileName = path.filename();
		const auto& extension = path.extension();

		std::shared_ptr<TextureHolder> imageTexture;
		if (it.is_directory())
		{
			imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/Folder.png");
		}
		else
		{

			if (extension == ".dds" || extension == ".png" || extension == ".hdr")
			{
				imageTexture = AssetManager::Get().LoadAsset<TextureHolder>(path);
			}
			else if (extension == ".fbx")
			{
				AssetManager::Get().ForceLoadAsset<TextureHolder>(std::format("INTERNAL_IMAGE_UI_{}",fileName.string()),imageTexture);
				
				std::shared_ptr<Mesh> mesh;
				AssetManager::Get().ForceLoadAsset<Mesh>(path,mesh);
				if (!imageTexture->isLoadedComplete && !imageTexture->isBeingLoaded)
				{
					GraphicsEngine::Get().RenderMRToTexture(mesh,imageTexture);
				}
				else if (imageTexture->isBeingLoaded || (!imageTexture->isLoadedComplete && !imageTexture->isBeingLoaded))
				{
					imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
				} 

			}
			else if (extension == ".json")
			{
				imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
			}
			else if (extension == ".cso")
			{
				imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
			}
			else
			{
				imageTexture = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
			}
		}

		ImGui::ImageButton(fileName.string().c_str(),imageTexture,{ cellWidth,cellWidth });


		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (it.is_directory())
			{
				m_CurrentPath /= fileName;
			}
			else
			{
				ShellExecute(0,0,it.path().wstring().c_str(),0,0,SW_SHOW);
			}
		}
		ImGui::TextWrapped(fileName.string().c_str());
		ImGui::NextColumn();
	}
	ImGui::End();
}
