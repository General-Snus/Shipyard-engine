#include "Engine/AssetManager/AssetManager.pch.h" 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/Math.hpp>
#include "../Transform.h"

#include "Tools/ImGui/ImGui/imgui.h"

Transform::Transform(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{ 
}

void Transform::Init()
{
}

void Transform::Update()
{
	OPTICK_EVENT();
	IsRecentlyUpdated = false;
	if (IsDirty)
	{
		MakeClean();
		this->GetGameObject().OnSiblingChanged(&typeid(Transform));
	}
}

void Transform::MakeClean()
{
	OPTICK_EVENT();
	IsRecentlyUpdated = true;
	MakeSaneRotation();
	myTransform =
		Matrix::CreateScaleMatrix(myScale) *
		Matrix::CreateRotationAroundX(myRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(myRotation.y * DEG_TO_RAD) *
		Matrix::CreateRotationAroundZ(myRotation.z * DEG_TO_RAD) *
		Matrix::CreateTranslationMatrix(myPosition);
	IsDirty = false;

#ifdef _DEBUGDRAW 
	DebugDrawer::Get().SetDebugPrimitiveTransform(primitive,myTransform);
#endif // _DEBUGDRAW 
}

void Transform::Render()
{
#ifdef _DEBUGDRAW 
	OPTICK_EVENT();

#endif // _DEBUGDRAW 
}
const Matrix& Transform::GetTransform()
{
	if (IsDirty)
	{
		MakeClean();
		this->GetGameObject().OnSiblingChanged(&typeid(Transform));
	}
	return myTransform;
}

Matrix& Transform::GetMutableTransform()
{
	return myTransform;
}

const Matrix& Transform::GetRawTransform() const
{
	return myTransform;
}

const Quaternionf& Transform::GetQuatF()
{
	return myQuaternion;
}

void Transform::SetQuatF(const Quaternionf& a_Rotator)
{
	myQuaternion = a_Rotator;
	myRotation = myQuaternion.GetEulerAngles();
	IsDirty = true;
}

Vector3f Transform::GetForward() const
{
	return Vector3f(myTransform(3,1),myTransform(3,2),myTransform(3,3)).GetNormalized();
}

Vector3f Transform::GetRight() const
{
	return Vector3f(myTransform(1,1),myTransform(1,2),myTransform(1,3)).GetNormalized();
}

Vector3f Transform::GetUp() const
{
	return Vector3f(myTransform(2,1),myTransform(2,2),myTransform(2,3)).GetNormalized();
}

void Transform::Move(Vector2f translation)
{
	myPosition.x += translation.x;
	myPosition.y += translation.y;
	IsDirty = true;
}
void Transform::Move(Vector3f translation)
{
	myPosition += translation;
	IsDirty = true;
}
void Transform::Move(float X,float Y,float Z)
{
	myPosition += {X,Y,Z};
	IsDirty = true;
}

void Transform::SetPosition(Vector2f position)
{
	//myTransform(4,1) = position.x;
	//myTransform(4,2) = position.y;
	myPosition.x = position.x;
	myPosition.y = position.y;
	IsDirty = true;
}
void Transform::SetPosition(Vector3f position)
{
	myPosition = position;
	IsDirty = true;
}
void Transform::SetPosition(float X,float Y,float Z)
{
	myPosition = { X,Y,Z };
	IsDirty = true;
}

Vector3f Transform::GetPosition() const
{
	return myPosition;
};
void Transform::MakeSaneRotation()
{
	for (int i = 0; i < 3; i++)
	{
		if (std::abs(myRotation[i]) > 360)
		{
			myRotation[i] = std::fmodf(myRotation[i],360);
		}
	}
}
void Transform::Rotate(float X,float Y,float Z)
{
	myRotation += {X,Y,Z};
	IsDirty = true;
}
void Transform::Rotate(Vector2f angularRotation)
{
	myRotation.x += angularRotation.x;
	myRotation.y += angularRotation.y;
	IsDirty = true;

	//if(worldSpace)
	//{
	//	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
	//		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) * myTransform;
	//}
	//else
	//{
	//	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
	//		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) * myTransform;
	//}
}
void Transform::Rotate(Vector3f angularRotation)
{
	myRotation += angularRotation;
	IsDirty = true;
}

/*
void Transform::ApplyTransformation(Matrix transformationMatrix)
{
	myTransform = transformationMatrix * myTransform;
}
*/

void Transform::SetScale(Vector3f scale)
{
	myScale = scale;
	IsDirty = true;
	//ApplyTransformation(Matrix::CreateScaleMatrix(scale));
}

Vector3f Transform::GetScale() const
{
	return myScale;
}

bool Transform::GetIsRecentlyUpdated() const
{
	return IsRecentlyUpdated;
}

bool Transform::GetIsDirty() const
{
	return IsDirty;
}

void Transform::SetGizmo(bool enabled)
{
	IsDebugGizmoEnabled = enabled;
	if (enabled)
	{
		primitive = DebugDrawer::Get().AddDebugGizmo(Vector3f(),1.0f);
		DebugDrawer::Get().SetDebugPrimitiveTransform(primitive,myTransform);
	}
	else
	{
		DebugDrawer::Get().RemoveDebugPrimitive(primitive);
	}
}

void DrawVec3Control(const std::string& label,Vector3f& values,float resetValue = 0.0f,float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0,columnWidth);
	ImGui::Text("%s",label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3,ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button,ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X",buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X",&values.x,0.1f,0.0f,0.0f,"%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button,ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y",buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y",&values.y,0.1f,0.0f,0.0f,"%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button,ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z",buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z",&values.z,0.1f,0.0f,0.0f,"%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

bool Transform::InspectorView()
{
	OPTICK_EVENT(); 
	if (!Component::InspectorView())
	{
		return false;
	}

	std::array<const char*,2> localGlobal = {"Relative", "World"};
	static int coordinateSpace1 = 0; 
	//ImGui::BeginColumns("##Transform",2,ImGuiOldColumnFlags_NoResize);
	//ImGui::SetColumnWidth(0,125);
	//
	//ImGui::Combo("##Location",&coordinateSpace1,localGlobal.data(),2); 
	//static int coordinateSpace2 = 0;
	//ImGui::Combo("##Rotation",&coordinateSpace2,localGlobal.data(),2); 
	//static int coordinateSpace3 = 0;
	//ImGui::Combo("##Scale",&coordinateSpace3,localGlobal.data(),2); 
	//ImGui::NextColumn();


	DrawVec3Control("Position",myPosition);
	DrawVec3Control("Euler angles",myRotation);
	DrawVec3Control("Scale",myScale);
	//IsDirty |= ImGui::DragFloat3("Position",&myPosition);
	//IsDirty |= ImGui::DragFloat3("Euler angles",&myRotation);
	//ImGui::DragFloat4("Quaternion",&myQuaternion,1,0,0,"%.4f",ImGuiSliderFlags_NoInput);
	//IsDirty |= ImGui::DragFloat3("Scale",&myScale);
	//ImGui::EndColumns();
	return true;
}

void Transform::SetRotation(float X,float Y,float Z)
{
	myRotation = { X,Y,Z };

	myQuaternion.SetEulerAngles(myRotation);
	IsDirty = true;
}

void Transform::SetRotation(Vector2f angularRotation)
{
	myRotation.x = angularRotation.x;
	myRotation.y = angularRotation.y;
	myQuaternion.SetEulerAngles(myRotation);
	IsDirty = true;
}

void Transform::SetRotation(Vector3f angularRotation)
{
	myRotation = angularRotation;
	myQuaternion.SetEulerAngles(myRotation);
	IsDirty = true;
}

void Transform::LookAt(Vector3f target)
{
	target;
	throw std::exception("Not implemented");
}

Vector3f Transform::GetRotation() const
{
	return myQuaternion.GetEulerAngles();
}

Vector3f Transform::VectorToEulerAngles(Vector3f input) const
{
	input;
	//Fuck euler angles remove this pos
	return Vector3f();
}

void Transform::SetScale(float X,float Y,float Z)
{
	myScale.x = X;
	myScale.y = Y;
	myScale.z = Z;
	IsDirty = true;
}

void Transform::SetScale(Vector2f scale)
{
	myScale.x = scale.x;
	myScale.y = scale.y;
	IsDirty = true;
}

void Transform::SetScale(float scale)
{
	myScale.x = scale;
	myScale.y = scale;
	myScale.z = scale;
	IsDirty = true;
	//ApplyTransformation(Matrix::CreateScaleMatrix({scale,scale,scale}));
}

bool Transform::HasChildren() const
{
	return !m_Children.empty();
}