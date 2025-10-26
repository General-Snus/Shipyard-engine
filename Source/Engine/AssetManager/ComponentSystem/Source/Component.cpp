#include "AssetManager.pch.h"

#include "../Component.h"
#include "../GameObject.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"

void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1), 0);
}

Component::Component(const SY::UUID anOwnerID, GameObjectManager* aManager)
	: myOwnerID(anOwnerID), myManager(aManager), m_IsActive(true)
{
}

Component::Component(const Component& aComponent) = default;

GameObject Component::gameObject() const
{
	return myManager ? myManager->GetGameObject(myOwnerID) : GameObject();
}
bool Component::IsActive() const
{
	return m_IsActive && myManager->GetActive(myOwnerID);
}
bool Component::InspectorView()
{
	return ComponentInspectorHeader();
}

FORCEINLINE bool Component::ComponentInspectorHeader()
{
	OPTICK_EVENT();

	bool isOpened = false;
	const Reflection::TypeInfo& typeInfo = GetTypeInfo();

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

const Transform& Component::transform() const
{
	return GetComponent<Transform>();
}
