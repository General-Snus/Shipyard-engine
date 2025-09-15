#include "AssetManager.pch.h"

#include "ReflectionTemplate.h"
#include "Editor/Editor/Helpers/ImGuiHelpers.h"
#include "Tools\ImGui\misc\cpp\imgui-combo-filter.h"

bool Reflection::ImGuiReflect(bool& ref, const std::string& identifier)
{
	return ImGui::Checkbox(std::format("##{}", identifier).c_str(), &ref);
}

bool Reflection::ImGuiReflect(const GameObject& ref, const std::string& identifier)
{
	identifier;
	ImGui::TextWrapped(std::format("{} {}", ref.GetName(), static_cast<std::string>(ref.GetID())).c_str());
	return false;
}

bool Reflection::ImGuiReflect(Color& ref, const std::string& identifier)
{
	bool returnArg = false;
	// Show selection button of all active blends,
	// if address of ref have a corresponding blend or mix, use that one,

	static std::vector<std::string> keys;
	keys.reserve(ColorManagerInstance.m_NamedColor.size());
	keys.clear();
	for (const auto& key : ColorManagerInstance.m_NamedColor | std::ranges::views::keys)
	{
		keys.push_back(key);
	}
	ImGui::Separator();
	ImGui::Text("Color preset menu");
	static bool blendColor = false;
	returnArg |= ImGui::Checkbox("Use blended color:", &blendColor);

	if (blendColor)
	{
		bool       changed = false;
		static int selected_item1 = -1;
		changed |= ComboAutoSelect("Blend value 1", selected_item1, keys, item_getter1, autoselect_search_vector,
								   ImGuiComboFlags_HeightSmall);

		static int selected_item2 = -1;
		changed |= ComboAutoSelect("Blend value 2", selected_item2, keys, item_getter1, autoselect_search_vector,
								   ImGuiComboFlags_HeightSmall);
		static float blendFactor = 0.5f;
		changed |= ImGui::SliderFloat("Blend", &blendFactor, 0.0f, 1.0f);
		ImGui::SameLine();
		const bool     var1 = selected_item1 == -1 || keys.size() < selected_item1;
		const bool     var2 = selected_item2 == -1 || keys.size() < selected_item2;
		const Vector4f blend1 =
			var1 ? Vector4f(1, 1, 1, 1) : ColorManagerInstance.GetColor(keys.at(selected_item1));
		const Vector4f blend2 =
			var2 ? Vector4f(1, 1, 1, 1) : ColorManagerInstance.GetColor(keys.at(selected_item2));

		const Vector4f color = Color::RGBLerp(blend1, blend2, blendFactor);
		if (changed)
		{
			ref.SetColor(color);
		}
		ImGui::ColorButton("Color:", color);
		returnArg = changed;
	}
	else
	{
		const int preSelected = static_cast<int>(std::distance(ColorManagerInstance.m_NamedColor.begin(),
			ColorManagerInstance.m_NamedColor.find(
				ref.m_ColorName)));

		static int selected_item1 = -1;

		selected_item1 = preSelected != ColorManagerInstance.m_NamedColor.size() ? preSelected : -1;

		if (ComboAutoSelect("Preset", selected_item1, keys, item_getter1, autoselect_search_vector,
			ImGuiComboFlags_HeightSmall) &&
			selected_item1 != -1)
		{
			returnArg = true;
			ref.m_ColorName = keys.at(selected_item1);
		}
		auto colorArray = ref.GetRGBA();
		if (ImGui::ColorEdit4(std::format("##{}", identifier).c_str(), &colorArray.x, ImGuiColorEditFlags_NoInputs))
		{
			returnArg = true;
			ref.SetColor(colorArray);
			selected_item1 = -1;
		}
	}
	return returnArg;
}

bool Reflection::ImGuiReflect(Vector2<float>& ref, const std::string& identifier)
{
	return ImGui::DragFloat2(std::format("##{}", identifier).c_str(), &ref.x);
}

bool Reflection::ImGuiReflect(Vector3<float>& ref, const std::string& identifier)
{
	return ImGui::DragFloat3(std::format("##{}", identifier).c_str(), &ref.x);
}

bool Reflection::ImGuiReflect(Vector4<float>& ref, const std::string& identifier)
{
	return ImGui::DragFloat4(std::format("##{}", identifier).c_str(), &ref.x);
}

bool Reflection::ImGuiReflect(float& ref, const std::string& identifier)
{
	return ImGui::DragFloat(std::format("##{}", identifier).c_str(), &ref);
}

bool Reflection::ImGuiReflect(int& ref, const std::string& identifier)
{
	return ImGui::DragInt(std::format("##{}", identifier).c_str(), &ref);
}
