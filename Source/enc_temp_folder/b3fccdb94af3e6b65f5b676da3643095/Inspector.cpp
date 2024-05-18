#include <Editor/Editor/Core/Editor.h>
#include "../Inspector.h"
#include "ComponentSystem/Components/Transform.h"
#include "imgui.h" 

void Inspector::RenderImGUi()
{

	ImGui::Begin("Inspector");
	const auto& selectedGameObjects = Editor::GetSelectedGameObjects();

	if (!selectedGameObjects.empty())
	{
		auto gameobject = selectedGameObjects[0];
		auto& tr = gameobject.GetComponent<Transform>();

		auto pos = tr.GetPosition();
		auto rot = tr.GetRotation();
		auto quat = tr.GetQuatF();
		auto scale = tr.GetScale();


		ImGui::DragFloat3("Position",&pos) ? tr.SetPosition(pos) : __nop();
		ImGui::DragFloat3("Euler angles",&rot) ? tr.SetRotation(rot) : __nop();
		ImGui::DragFloat4("Quaternion",&quat,0.01f,0.f,1.f) ? tr.SetQuatF(quat.GetNormalized()) : __nop();
		ImGui::DragFloat3("Scale",&scale) ? tr.SetScale(scale) : __nop();

	}
	ImGui::End();
}
