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

	refl::util::for_each(refl::reflect<Component>().members,[&](auto member) {
		std::string arg = std::string(get_display_name(member)) + ": ";
		arg += refl::runtime::debug_str(member(*this));
		ImGui::Text(arg.c_str());
		});

}
