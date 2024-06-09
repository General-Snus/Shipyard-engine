#include "AssetManager.pch.h"

#include <cmath>
#include "../Component.h"


void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1),0);
}

__forceinline void Component::InspectorView()
{
	const TypeInfo& typeInfo = this->GetTypeInfo();
	ImGui::Checkbox(std::format("##{}{}",typeInfo.Name().c_str(),std::to_string(myOwnerID).c_str()).c_str(),&m_IsActive);
	ImGui::SameLine(0,10);
	ImGui::Text(typeInfo.Name().c_str()); 
	ImGui::SameLine();
}
