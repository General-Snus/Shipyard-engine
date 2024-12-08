#include "AssetManager.pch.h"

#include "../Transform.h"
#include <Tools/Utilities/Math.hpp>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

#include <Tools/ImGui/ImGuiHelpers.hpp>
#include "Tools/ImGui/imgui.h"

#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Editor/Editor/Commands/VarChanged.h>

Transform::Transform(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
}

void Transform::Destroy()
{
	Component::Destroy();
	Detach();
	RemoveAllChildren();
}

void Transform::Init()
{
	if (m_Parent.IsValid())
	{
		m_Parent.transform().AddChild(*this);
	}

	if (!m_Children.empty())
	{
		const auto children = m_Children;
		m_Children.clear();
		for (const auto& child : children)
		{
			if (child.IsValid())
			{
				child.transform().SetParent(*this);
			}
		}
	}
}

void Transform::Update()
{
	OPTICK_EVENT();
	IsRecentlyUpdated = false;
#if DoDirtyChecks
    if (GetIsDirty())
    {
        MakeClean();
        const auto obj = GetGameObject();
        if (obj.IsValid())
        {
            obj.OnSiblingChanged(&typeid(Transform));
        }
    }
#else
	MakeClean();

	const auto obj = GetGameObject();
	if (obj.IsValid())
	{
		obj.OnSiblingChanged(&typeid(Transform));
	}
#endif //
}

void Transform::SetDirty(bool arg)
{
	arg;
#if DoDirtyChecks
    IsDirty = arg;
#endif
}

void Transform::MakeClean()
{
	OPTICK_EVENT();
	IsRecentlyUpdated = true;
	MakeSaneRotation();
	myTransform = Matrix::CreateScaleMatrix(myScale) * Matrix::CreateRotationAroundX(myRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(myRotation.y * DEG_TO_RAD) *
		Matrix::CreateRotationAroundZ(myRotation.z * DEG_TO_RAD) *
		Matrix::CreateTranslationMatrix(myPosition);
	SetDirty(false);

	if (HasParent())
	{
		myWorldSpaceTransform = GetTransform() * m_Parent.transform().WorldMatrix();
	}
	else
	{
		myWorldSpaceTransform = myTransform;
	}

	for (auto& child : m_Children)
	{
		child.transform().MakeClean();
	}

#ifdef _DEBUGDRAW
    DebugDrawer::Get().SetDebugPrimitiveTransform(primitive, myTransform);
#endif // _DEBUGDRAW
}

const Matrix& Transform::GetTransform()
{
	if (GetIsDirty())
	{
		MakeClean();
		// TODO FIX ALL YOUR SHIT
		// this->GetGameObject().OnSiblingChanged(&typeid(Transform));
	}
	return myTransform;
}

const Matrix& Transform::WorldMatrix() const
{
	if (!HasParent())
	{
		return myTransform;
	}
	return myWorldSpaceTransform;
}

const Matrix& Transform::WorldMatrix()
{
	if (!HasParent())
	{
		return GetTransform();
	}
	const auto transform = m_Parent.transform();
	if (transform.GetIsDirty() || GetIsDirty())
	{
		myWorldSpaceTransform = m_Parent.transform().WorldMatrix() * GetTransform();
	}

	return myWorldSpaceTransform;
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
	myRotation = myQuaternion.GetEulerAngles() * RAD_TO_DEG;
	SetDirty(true);
}

Vector3f Transform::GetForward(eSpace space) const
{
	if (space == LOCAL)
	{
		return Vector3f(myTransform(3, 1), myTransform(3, 2), myTransform(3, 3)).GetNormalized();
	}
	if (space == WORLD)
	{
		return Vector3f(myWorldSpaceTransform(3, 1), myWorldSpaceTransform(3, 2), myWorldSpaceTransform(3, 3))
			.GetNormalized();
	}

	return Vector3f(myTransform(3, 1), myTransform(3, 2), myTransform(3, 3)).GetNormalized();
}

Vector3f Transform::GetRight(eSpace space) const
{
	if (space == LOCAL)
	{
		return Vector3f(myTransform(1, 1), myTransform(1, 2), myTransform(1, 3)).GetNormalized();
	}
	if (space == WORLD)
	{
		return Vector3f(myWorldSpaceTransform(1, 1), myWorldSpaceTransform(1, 2), myWorldSpaceTransform(1, 3))
			.GetNormalized();
	}

	return Vector3f(myTransform(1, 1), myTransform(1, 2), myTransform(1, 3)).GetNormalized();
}

Vector3f Transform::GetUp(eSpace space) const
{
	if (space == LOCAL)
	{
		return Vector3f(myTransform(2, 1), myTransform(2, 2), myTransform(2, 3)).GetNormalized();
	}
	if (space == WORLD)
	{
		return Vector3f(myWorldSpaceTransform(2, 1), myWorldSpaceTransform(2, 2), myWorldSpaceTransform(2, 3))
			.GetNormalized();
	}

	return Vector3f(myTransform(2, 1), myTransform(2, 2), myTransform(2, 3)).GetNormalized();
}

void Transform::Move(Vector2f translation)
{
	myPosition.x += translation.x;
	myPosition.y += translation.y;
	SetDirty(true);
}

void Transform::Move(Vector3f translation)
{
	myPosition += translation;
	SetDirty(true);
}

void Transform::Move(float X, float Y, float Z)
{
	myPosition += {X, Y, Z};
	SetDirty(true);
}

void Transform::SetPosition(Vector2f position)
{
	// myTransform(4,1) = position.x;
	// myTransform(4,2) = position.y;
	myPosition.x = position.x;
	myPosition.y = position.y;
	SetDirty(true);
}

void Transform::SetPosition(Vector3f position)
{
	myPosition = position;
	SetDirty(true);
}

void Transform::SetPosition(float X, float Y, float Z)
{
	myPosition = {X, Y, Z};
	SetDirty(true);
}

Vector3f Transform::GetPosition(eSpace space) const
{
	if (space == LOCAL)
	{
		return myPosition;
	}
	if (space == WORLD)
	{
		return Vector3f(myWorldSpaceTransform(4, 1), myWorldSpaceTransform(4, 2), myWorldSpaceTransform(4, 3));
	}

	return myPosition;
}

Vector3f& Transform::localPosition()
{
	return myPosition;
};

void Transform::MakeSaneRotation()
{
	for (int i = 0; i < 3; i++)
	{
		if (std::abs(myRotation[i]) > 360)
		{
			myRotation[i] = std::fmodf(myRotation[i], 360);
		}
	}
}

void Transform::Rotate(float X, float Y, float Z)
{
	myRotation += {X, Y, Z};
	SetDirty(true);
}

void Transform::Rotate(Vector2f angularRotation)
{
	myRotation.x += angularRotation.x;
	myRotation.y += angularRotation.y;
	SetDirty(true);
	// if(worldSpace)
	//{
	//	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
	//		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) * myTransform;
	// }
	// else
	//{
	//	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
	//		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) * myTransform;
	// }
}

void Transform::Rotate(Vector3f angularRotation)
{
	myRotation += angularRotation;
	SetDirty(true);
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
	SetDirty(true);
	// ApplyTransformation(Matrix::CreateScaleMatrix(scale));
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
#if DoDirtyChecks
    return IsDirty;
#endif
	return false;
}

void Transform::SetGizmo(bool enabled)
{
	IsDebugGizmoEnabled = enabled;
	if (enabled)
	{
		primitive = DebugDrawer::Get().AddDebugGizmo(Vector3f(), 1.0f);
		DebugDrawer::Get().SetDebugPrimitiveTransform(primitive, myTransform);
	}
	else
	{
		DebugDrawer::Get().RemoveDebugPrimitive(primitive);
	}
}

bool DrawVec3Control(const std::string& label, Vector3f& values, float resetValue = 0.0f, float columnWidth = 100.0f,
                     bool*              justStoppedInteraction = nullptr)
{
	resetValue; // Leave this here becuase prefabs might want this?
	bool changed = false;
	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	const float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	const ImVec2 buttonSize = {lineHeight + 3.0f, lineHeight};

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
	ImGui::Button("X", buttonSize);
	ImGui::PopStyleColor(1);

	ImGui::SameLine();
	changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
	if (ImGui::IsItemJustReleased())
	{
		*justStoppedInteraction = true;
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
	ImGui::Button("Y", buttonSize);
	ImGui::PopStyleColor(1);

	ImGui::SameLine();
	changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
	if (ImGui::IsItemJustReleased())
	{
		*justStoppedInteraction = true;
	}
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
	ImGui::Button("Z", buttonSize);
	ImGui::PopStyleColor(1);

	ImGui::SameLine();
	changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		*justStoppedInteraction = true;
	}
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
	return (changed || *justStoppedInteraction);
}

bool Transform::InspectorView()
{
	OPTICK_EVENT();
	if (!Component::InspectorView())
	{
		return false;
	}

	constexpr std::array<const char*, 2> localGlobal = {"Relative", "World"};
	static int                           coordinateSpace1 = 0;
	ImGui::BeginColumns("##Transform", 2, ImGuiOldColumnFlags_NoResize);
	ImGui::Combo("##Location", &coordinateSpace1, localGlobal.data(), 2);
	static int coordinateSpace2 = 0;
	ImGui::Combo("##Rotation", &coordinateSpace2, localGlobal.data(), 2);
	static int coordinateSpace3 = 0;
	ImGui::Combo("##Scale", &coordinateSpace3, localGlobal.data(), 2);
	ImGui::NextColumn();

	bool justReleased = false;
	auto oldValue = myPosition;
	if (DrawVec3Control("Position", myPosition, 0, 100, &justReleased))
	{
		CommandBuffer::mainEditorCommandBuffer().addCommand<VarChanged<Transform, Vector3f>>(
			this, &myPosition, oldValue, myPosition, "Position");
		if (justReleased)
		{
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		SetDirty(true);
	}

	justReleased = false;
	oldValue = myRotation;
	if (DrawVec3Control("Euler angles", myRotation, 0, 100, &justReleased))
	{
		CommandBuffer::mainEditorCommandBuffer().addCommand<VarChanged<Transform, Vector3f>>(
			this, &myRotation, oldValue, myRotation, "Rotation");

		if (justReleased)
		{
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}

		SetDirty(true);
	}

	justReleased = false;
	oldValue = myScale;
	if (DrawVec3Control("Scale", myScale, 1.0f, 100, &justReleased))
	{
		CommandBuffer::mainEditorCommandBuffer().addCommand<VarChanged<Transform, Vector3f>>(
			this, &myScale, oldValue, myScale, "Scale");
		if (justReleased)
		{
			CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
		}
		SetDirty(true);
	}
	return true;
}

void Transform::SetRotation(float X, float Y, float Z)
{
	myRotation = {X, Y, Z};

	myQuaternion.SetEulerAngles(myRotation);
#if DoDirtyChecks
    IsDirty = true;
#endif
}

void Transform::SetRotation(Vector2f angularRotation)
{
	myRotation.x = angularRotation.x;
	myRotation.y = angularRotation.y;
	myQuaternion.SetEulerAngles(myRotation);
	SetDirty(true);
}

void Transform::SetRotation(Vector3f angularRotation)
{
	myRotation = angularRotation;
	myQuaternion.SetEulerAngles(myRotation);
	SetDirty(true);
}

Vector3f& Transform::localRotation()
{
	SetDirty(true);
	return myRotation;
}

void Transform::LookAt(Vector3f target, Vector3f Up)
{
	Up;
	myQuaternion = Quaternionf::LookAt(myPosition, target, GetForward(WORLD), GetUp(WORLD));
	myRotation = myQuaternion.GetEulerAngles() * RAD_TO_DEG;
	SetDirty(true);
}

Vector3f Transform::GetRotation() const
{
	return myRotation;
	// return myQuaternion.GetEulerAngles() * DEG_TO_RAD;
}

Vector3f Transform::VectorToEulerAngles(Vector3f input) const
{
	input;
	// Fuck euler angles remove this pos
	return Vector3f();
}

void Transform::SetScale(float X, float Y, float Z)
{
	myScale.x = X;
	myScale.y = Y;
	myScale.z = Z;
	SetDirty(true);
}

void Transform::SetScale(Vector2f scale)
{
	myScale.x = scale.x;
	myScale.y = scale.y;
	SetDirty(true);
}

void Transform::SetScale(float scale)
{
	myScale.x = scale;
	myScale.y = scale;
	myScale.z = scale;

	SetDirty(true);
	// ApplyTransformation(Matrix::CreateScaleMatrix({scale,scale,scale}));
}

std::string Transform::GetParentName() const
{
	if (HasParent())
	{
		return m_Parent.GetName();
	}
	return "";
}

bool Transform::SetParent(Transform& parent)
{
	return SetParent(parent, false);
}

bool Transform::SetParent(Transform& parent, bool worldPositionStays)
{
	UNREFERENCED_PARAMETER(worldPositionStays);
	// TODO make sure position stays after parenting
	const auto obj = parent.GetGameObject();
	if (obj.IsValid() && obj.GetID() != myOwnerID)
	{
		Detach();
		m_Parent = obj;
		m_Parent.transform().AddChild(*this);
		return true;
	}
	LOGGER.Err("Parent Not set, gameobject was invalid");
	return false;
}

Transform& Transform::Root()
{
	Transform& iterator = *this;
	while (iterator.HasParent())
	{
		iterator = iterator.GetParent();
	}
	return iterator;
}

bool Transform::HasParent() const
{
	return m_Parent.IsValid();
}

Transform& Transform::GetParent() const
{
	if (HasParent())
	{
		return m_Parent.GetComponent<Transform>();
	}
	LOGGER.Critical("Error: No parent on " + GetGameObject().GetName());
	throw;
}

bool Transform::Find(const std::string& nameOfChild, Transform& transform) const
{
	for (auto& child : m_Children)
	{
		if (child.GetName() == nameOfChild)
		{
			transform = child.GetComponent<Transform>();
			return true;
		}
	}
	return false;
}

bool Transform::FindRecursive(const std::string& nameOfChild, Transform& transform) const
{
	for (auto& child : m_Children)
	{
		if (child.GetName() == nameOfChild)
		{
			transform = child.GetComponent<Transform>();
			return true;
		}
	}

	for (auto& child : m_Children)
	{
		if (child.GetComponent<Transform>().FindRecursive(nameOfChild, transform))
		{
			return true;
		}
	}
	return false;
}

Transform& Transform::GetChild(int index) const
{
	if (m_Children.size() > index)
	{
		return m_Children[index].GetComponent<Transform>();
	}
	LOGGER.Critical("Error: No Child found on " + GetGameObject().GetName() + " at index " + std::to_string(index));
	throw;
}

bool Transform::HasChild(const SY::UUID id) const
{
	for (auto& c : m_Children)
	{
		if (c.GetID() == id)
		{
			return true;
		}
	}
	return false;
}

bool Transform::HasChild(const GameObject& obj) const
{
	return HasChild(obj.GetID());
}

bool Transform::HasChildren() const
{
	return !m_Children.empty();
}

unsigned int Transform::GetChildCount() const
{
	return static_cast<unsigned>(m_Children.size());
}

// This is probable very very expensive
std::vector<std::reference_wrapper<Transform>> Transform::GetAllChildren()
{
	std::vector<std::reference_wrapper<Transform>> childTransforms;

	for (auto& child : m_Children)
	{
		auto& ref = child.GetComponent<Transform>();
		childTransforms.emplace_back(ref);

		auto refVector = ref.GetAllChildren();
		childTransforms.reserve(childTransforms.size() + refVector.size());
		childTransforms.insert(childTransforms.end(), refVector.begin(), refVector.end());
	}

	return childTransforms;
}

std::vector<std::reference_wrapper<Transform>> Transform::GetAllDirectChildren() const
{
	std::vector<std::reference_wrapper<Transform>> childTransforms;

	for (auto& child : m_Children)
	{
		auto& ref = child.GetComponent<Transform>();
		childTransforms.emplace_back(ref);
	}
	return childTransforms;
}

bool Transform::AddChild(Transform& child)
{
	const auto id = child.GetGameObject().GetID();

	if (id.IsValid())
	{
		child.Detach();
		child.m_Parent = GetGameObject();
		m_Children.emplace_back(child.GetGameObject());
		child.myWorldSpaceTransform = child.WorldMatrix();
		return true;
	}
	LOGGER.Err("Child not set, child is not valid gameobject");
	child.myWorldSpaceTransform = child.WorldMatrix();
	return false;
}

bool Transform::AddChildren(std::vector<std::reference_wrapper<Transform>>& childs)
{
	bool returnArgument = false;
	for (auto& child : childs)
	{
		returnArgument |= AddChild(child);
	}
	return returnArgument;
}

bool Transform::RemoveChild(Transform& child)
{
	const auto id = child.myOwnerID;
	int        indexToRemove = -1;

	int index = 0;
	for (auto& c : m_Children)
	{
		if (c.GetID() == id)
		{
			indexToRemove = index;
			break;
		}
		index++;
	}

	if (indexToRemove != -1)
	{
		m_Children[indexToRemove].GetComponent<Transform>().RemoveAllChildren();
		myManager->DeleteGameObject(m_Children[indexToRemove]);
		m_Children.erase(m_Children.begin() + indexToRemove);
		return true;
	}

	return false;
}

void Transform::RemoveAllChildren()
{
	for (auto& c : m_Children)
	{
		c.GetComponent<Transform>().RemoveAllChildren();
		myManager->DeleteGameObject(myOwnerID);
	}
}

void Transform::SetChildrenActive(bool isActive) const
{
	for (auto& c : m_Children)
	{
		c.SetActive(isActive);
	}
}

void Transform::SetChildrenActiveRecursive(bool isActive) const
{
	for (auto& c : m_Children)
	{
		c.SetActive(isActive);
		c.GetComponent<Transform>().SetChildrenActiveRecursive(isActive);
	}
}

bool Transform::IsChildOf(Transform& parent) const
{
	return parent.myOwnerID == m_Parent.GetID();
}

void Transform::DetachChildren()
{
	for (auto& c : m_Children)
	{
		c.GetComponent<Transform>().Detach();
	}
	m_Children.clear();
}

void Transform::DetachChildren(int index)
{
	if (m_Children.size() > index)
	{
		m_Children[index].GetComponent<Transform>().Detach();
		m_Children.erase(m_Children.begin() + index);
	}
}

void Transform::DetachChildID(const SY::UUID index)
{
	for (auto& c : m_Children)
	{
		if (c.GetID() == index)
		{
			c = m_Children.back();
			m_Children.pop_back();
		}
	}
}

void Transform::DetachChild(const GameObject& index)
{
	DetachChildID(index.GetID());
}

void Transform::Detach()
{
	if (HasParent())
	{
		m_Parent.transform().DetachChildID(myOwnerID);
		m_Parent = GameObject();
	}
}
