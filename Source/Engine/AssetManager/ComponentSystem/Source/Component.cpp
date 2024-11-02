#include "AssetManager.pch.h"

#include "../Component.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"

void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1), 0);
}

bool Component::InspectorView()
{
	OPTICK_EVENT();

	bool isOpened = false;
	const TypeInfo &typeInfo = this->GetTypeInfo();
	ImGui::Checkbox(std::format("##{}{}", typeInfo.Name().c_str(), std::to_string(myOwnerID).c_str()).c_str(),
					&m_IsActive);
	ImGui::SameLine(0, 10);
	isOpened =
		ImGui::CollapsingHeader(typeInfo.Name().c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen |
															 ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick);

	if (isOpened)
	{
		ImGui::Indent();
		ImGui::Text("Owner: %s", std::string(myOwnerID).c_str());
		ImGui::Text("Inherited: %d", IsInherited);
		ImGui::Unindent();
	}

	return isOpened;
}

Transform &Component::transform()
{
	return GetComponent<Transform>();
}
