#include "../ColorPresets.h"

#include "Tools/ImGui/misc/cpp/imgui-combo-filter.h"
#include "Tools/ImGui/misc/cpp/imgui_stdlib.h"
#include "Tools/Utilities/Color.h"
#include "imgui.h"
#include <Tools/Optick/include/optick.h>
#include <ranges>

void ColorPresets::RenderImGUi()
{
    OPTICK_EVENT();
    ImGui::Begin("ColorPresets", &m_KeepWindow);
    {
        ImGui::Columns(3);
        ImGui::SetColumnWidth(0, 200);
        ImGui::SetColumnWidth(1, 50);
        ImGui::SetColumnWidth(2, 150);
        for (auto &[Name, Value] : ColorManagerInstance.m_NamedColor)
        {
            ImGui::Text(Name.c_str());
            ImGui::NextColumn();
            ImGui::ColorEdit4(("##" + Name).c_str(), &Value.x, ImGuiColorEditFlags_NoInputs);
            ImGui::NextColumn();
            ImGui::Text("Remove color: ");
            ImGui::SameLine();
            if (ImGui::Button(("##Delete" + Name).c_str(), {25, 25}))
            {
                ColorManagerInstance.m_NamedColor.erase(Name);
            }
            ImGui::Separator();
            ImGui::NextColumn();
        }
        ImGui::EndColumns();
        ImGui::InputText("Color name:", &activeColorAdd);
        ImGui::Checkbox("Blend color:", &blendColor);
        if (!blendColor)
        {
            ImGui::SameLine();
            ImGui::ColorEdit4("Color:", &activeColor.x, ImGuiColorEditFlags_NoInputs);
        }
        else
        {

            static std::vector<std::string> keys;
            keys.reserve(ColorManagerInstance.m_NamedColor.size());
            keys.clear();
            for (const auto &key : ColorManagerInstance.m_NamedColor | std::views::keys)
            {
                keys.push_back(key);
            }

            static int selected_item1 = -1;
            if (ImGui::ComboAutoSelect("Blend value 1", selected_item1, keys, item_getter1, autoselect_search_vector,
                                       ImGuiComboFlags_HeightSmall))
            {
            }

            static int selected_item2 = -1;
            if (ImGui::ComboAutoSelect("Blend value 2", selected_item2, keys, item_getter1, autoselect_search_vector,
                                       ImGuiComboFlags_HeightSmall))
            {
            }

            ImGui::SliderFloat("Blend", &blendFactor, 0.0f, 1.0f);
            ImGui::SameLine();

            Vector4f blend1 =
                selected_item1 == -1 ? Vector4f(1, 1, 1, 1) : ColorManagerInstance.GetColor(keys.at(selected_item1));
            Vector4f blend2 =
                selected_item2 == -1 ? Vector4f(1, 1, 1, 1) : ColorManagerInstance.GetColor(keys.at(selected_item2));

            activeColor = Color::RGBLerp(blend1, blend2, blendFactor);
            ImGui::ColorButton("Color:", activeColor);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save color"))
        {
            ColorManagerInstance.CreateColor(activeColorAdd, activeColor);
            activeColorAdd = "";
            activeColor = Vector4f();
        }
    }
    ImGui::End();
}
