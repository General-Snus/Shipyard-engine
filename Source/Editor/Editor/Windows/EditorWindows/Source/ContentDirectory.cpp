#define NOMINMAX
#include "../ContentDirectory.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Tools/ImGui/ImGui/ImGuiHepers.hpp>
#pragma optimize("",off)

ContentDirectory::ContentDirectory() : m_CurrentPath(AssetManager::AssetPath)
{
}

void ContentDirectory::RenderImGUi()
{
	{
		ImGui::Begin("ContentFolder");

		if (m_CurrentPath != AssetManager::AssetPath && ImGui::ArrowButton("##BackButton",ImGuiDir_Up))
		{
			m_CurrentPath = m_CurrentPath.parent_path();
		}

		const float contentFolderWidth = ImGui::GetContentRegionAvail().x;
		const auto cellWidth = 128.f;
		auto columnCount = static_cast<int>(contentFolderWidth / cellWidth);


		ImGui::Columns(std::max(columnCount,1),0);
		for (const auto& it : std::filesystem::directory_iterator(m_CurrentPath))
		{
			const auto& path = it.path();
			const auto& fileName = path.filename();




			if (it.is_directory())
			{
				const auto& folderAsset = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/Folder.png");
				ImGui::ImageButton(fileName.string().c_str(),folderAsset,{ cellWidth,cellWidth });
			}
			else
			{
				const auto& folderAsset = AssetManager::Get().LoadAsset<TextureHolder>("Textures/Widgets/File.png");
				ImGui::ImageButton(fileName.string().c_str(),folderAsset,{ cellWidth,cellWidth });
			}
			 

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (it.is_directory())
				{
					m_CurrentPath /= fileName;
				}
			}
			ImGui::Text(fileName.string().c_str());





			ImGui::NextColumn();
		}


		ImGui::End();
	}
}
