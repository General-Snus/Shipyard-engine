#include "AssetManager.pch.h"
#include <cmath>
#include "../Component.h"


void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1),0);
}

void Component::InspectorView()
{
	const TypeInfo& typeInfo = this->GetTypeInfo(); // get custom type info 
	ImGui::Text(typeInfo.Name().c_str());
}
