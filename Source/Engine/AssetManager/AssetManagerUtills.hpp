#pragma once
#include <Editor/Editor/Core/Editor.h>
#include <Editor/Editor/Windows/EditorWindows/CustomFuncWindow.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Tools/ImGui/imgui.h>
#include <functional>
#include <memory>

#include "Tools/Utilities/Math.hpp"
template <typename T>
concept SupportOwnWindowPtr = requires(T a) { a->InspectorView(); };
template <typename T>
concept SupportOwnWindow = requires(T a) { a.InspectorView(); };

template <typename asset = Mesh> void PopUpContextForAsset(std::shared_ptr<asset> &replace)
{
    ImGui::SetNextWindowSize({0, 200});
    if (ImGui::BeginPopupContextItem())
    {
        static char buf[128] = "";
        ImGui::InputText("Search", buf, IM_ARRAYSIZE(buf));
        const std::string keyTerm = buf;

        ImGui::BeginChild("test");
        const auto &assetMap = EngineResources.GetLibraryOfType<asset>()->GetContentCatalogue<asset>();

        using localPair = std::pair<std::string, std::shared_ptr<asset>>;
        static std::vector<localPair> sortedList;
        for (const auto &[path, content] : assetMap)
        {
            if (!content)
            {
                continue;
            }

            const std::string name = path.filename().string();
            if (!keyTerm.empty() && Levenstein::Distance(name, keyTerm) >= Levenstein::MaxSize(name, keyTerm))
            {
                continue;
            }

            sortedList.emplace_back(name, content);
        }

        if (!keyTerm.empty())
        {
            std::ranges::sort(sortedList, [=](const localPair &a, const localPair &b) {
                return Levenstein::Distance(a.first, keyTerm) < Levenstein::Distance(b.first, keyTerm);
            });
        }

        ImGui::BeginTable(replace->GetTypeInfo().Name().c_str(), 2);
        ImGui::TableSetColumnWidth(0, 50);
        ImGui::TableSetColumnWidth(1, 125);
        ImGuiListClipper clipper;
        clipper.Begin((int)sortedList.size());
        while (clipper.Step())
        {
            ImGui::TableNextColumn();
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                const auto &[path, content] = sortedList[row];
                ImGui::TextWrapped(path.c_str());
                ImGui::TableNextColumn();
                if (ImGui::ImageButton(("PopUpContextMenu" + path).c_str(), content->GetEditorIcon(), {100, 100}))
                {
                    replace = content;
                    ZeroMemory(buf, 128);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::TableNextColumn();
            }
        }
        ImGui::EndTable();
        ImGui::EndChild();
        ImGui::EndPopup();
        sortedList.clear();
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
                EditorInstance.g_EditorWindows.emplace_back(newWindow);
            }
        }
    }
    else
    {
        std::shared_ptr<TextureHolder> texture =
            EngineResources.LoadAsset<TextureHolder>("Textures\\Widgets\\File.png");
        if (!texture)
        {
            texture = GraphicsEngineInstance.GetDefaultTexture(eTextureType::MaterialMap);
        }

        ImGui::ImageButton((char *)&texture, texture, {100, 100}, ImGuiButtonFlags_PressedOnDoubleClick);
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(PayloadType.c_str()))
        {
            const auto path = std::string((const char *)payload->Data, payload->DataSize);
            asset = EngineResources.LoadAsset<assetType>(path);
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
//	friend class EngineResourceLoader;
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
