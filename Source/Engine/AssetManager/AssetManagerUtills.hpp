#pragma once
#include <Editor/Editor/Core/Editor.h>
#include <Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Tools/ImGui/ImGui/imgui.h>
#include <functional>
#include <memory>

template <typename T>
concept SupportOwnWindowPtr = requires(T a) { a->InspectorView(); };
template <typename T>
concept SupportOwnWindow = requires(T a) { a.InspectorView(); };

template <typename asset = Mesh> void PopUpContextForAsset(std::shared_ptr<asset> &replace)
{
    if (ImGui::BeginPopupContextItem())
    {
        ImGui::BeginTable(replace->GetTypeInfo().Name().c_str(), 2, 0, {0, 500});
        ImGui::TableNextColumn();
        const auto &assetMap = AssetManager::Get().GetLibraryOfType<asset>()->GetContentCatalogue<asset>();
        for (const auto &[path, content] : assetMap)
        {
            if (!content)
            {
                continue;
            }

            ImGui::Text(path.stem().string().c_str());
            ImGui::TableNextColumn();
            if (ImGui::ImageButton(("PopUpContextMenu" + path.string()).c_str(), content->GetEditorIcon(), {100, 100}))
            {
                replace = AssetManager::Get().LoadAsset<asset>(path);
                ImGui::CloseCurrentPopup();
            }
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
        ImGui::EndPopup();
    }
}

template <typename assetType = Mesh>
void SwitchableAsset(std::shared_ptr<assetType> &asset, std::string PayloadType, bool supportInspector = true)
{

    if (asset)
    {
        if (ImGui::ImageButton((char *)&asset, asset->GetEditorIcon(), {100, 100},
                               ImGuiButtonFlags_PressedOnDoubleClick) &&
            supportInspector)
        {

            if constexpr (SupportOwnWindow<std::shared_ptr<assetType>> ||
                          SupportOwnWindowPtr<std::shared_ptr<assetType>>)
            {
                auto newWindow = std::make_shared<CustomFuncWindow>(std::bind(&assetType::InspectorView, asset));
                newWindow->SetWindowName(asset->GetAssetPath().filename().string());
                Editor::Get().g_EditorWindows.emplace_back(newWindow);
            }
        }
    }
    else
    {
        std::shared_ptr<TextureHolder> texture =
            AssetManager::Get().LoadAsset<TextureHolder>("Textures\\Widgets\\File.png");
        if (!texture)
        {
            texture = GraphicsEngine::Get().GetDefaultTexture(eTextureType::MaterialMap);
        }

        ImGui::ImageButton((char *)&texture, texture, {100, 100}, ImGuiButtonFlags_PressedOnDoubleClick);
    }

    if (ImGui::BeginDragDropTarget())
    {

        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(PayloadType.c_str()))
        {
            const auto path = std::string((const char *)payload->Data, payload->DataSize);
            asset = AssetManager::Get().LoadAsset<assetType>(path);
        }
        ImGui::EndDragDropTarget();
    }
    PopUpContextForAsset<assetType>(asset);
}

// enum eCallbackAction
//{
//	loaded,unloaded,deleted,changed,renamed,created,count
// };
//
// class AssetCallbackMaster
//{
//	friend class AssetManager;
// public:
//	struct Message
//	{
//		eCallbackAction event;
//		std::type_info* type;
//		const std::filesystem::path& path;
//	};
// private:
//	struct dataStruct
//	{
//		//std::function<void(Message)> callbacksFunction;
//		SY::UUID subscribed;
//	};
//
// private:
//	template<typename T>
//	void UpdateStatusOf(const std::filesystem::path& path,eCallbackAction event);
//	AssetCallbackMaster() = default;
//	//activate on path, send to function call universal, need uuid and send action
//	std::unordered_map<std::filesystem::path,std::vector<dataStruct>> callbacks;
// };
