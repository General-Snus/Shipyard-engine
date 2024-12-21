#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

enum eSpace
{
	WORLD,
	LOCAL
};

#define DoDirtyChecks                                                                                                  \
    true //  allows or disallows isDirty, to get tasks done, TODO create a event manager for dirty events checks.

// LEFTHANDED X RIGHT Y UP Z FORWARD AS GOD INTENDED
class Transform : public Component
{
public:
	ReflectableTypeRegistration();
	Transform() = delete;
	Transform(SY::UUID anOwnerId, GameObjectManager* aManager);
	void Destroy() override;
	void Init() override;
	void Update() override;
	// Be ware, the matrix is built by the transform, changes wont carry but you can use this to avoid making copies of
	// the orignal matrix
	Matrix&       GetMutableTransform();
	const Matrix& LocalMatrix();
	const Matrix& WorldMatrix() const;
	const Matrix& WorldMatrix();

	// not dirty checked
	const Matrix&      GetRawTransform() const;
	const Quaternionf& GetQuatF() const ;
	void               SetQuatF(const Quaternionf&);

	// TODO
	// SET A UNIVERSAL METER
	Vector3f GetForward(eSpace space = LOCAL) const;
	Vector3f GetRight(eSpace space = LOCAL) const;
	Vector3f GetUp(eSpace space = LOCAL) const;

	void Move(Vector2f translation);
	void Move(Vector3f translation);
	void Move(float X, float Y, float Z);

	void      SetPosition(Vector2f position);
	void      SetPosition(Vector3f position);
	void      SetPosition(float X, float Y, float Z);
	Vector3f  GetPosition(eSpace space = LOCAL) const;
	Vector3f& localPosition();

	void      Rotate(float X, float Y, float Z);
	void      Rotate(Vector2f angularRotation);
	void      Rotate(Vector3f angularRotation);
	void      SetRotation(float X, float Y, float Z);
	void      SetRotation(Vector2f angularRotation);
	void      SetRotation(Vector3f angularRotation);
	Vector3f& localRotation();
	void      LookAt(Vector3f target, Vector3f Up = GlobalUp);

	//"You will eventually regret any use of Euler angles."
	// John Carmack
	Vector3f GetRotation() const;
	// Not mathematicly sound
	Vector3f VectorToEulerAngles(Vector3f input) const;

	void SetScale(float X, float Y, float Z);

	void     SetScale(float scale);
	void     SetScale(Vector2f scale);
	void     SetScale(Vector3f scale);
	Vector3f GetScale() const;

	// Meaning last frame
	bool GetIsRecentlyUpdated() const;
	bool GetIsDirty() const;

	~Transform() override = default;
	void SetGizmo(bool enabled);
	void InitPrimitive();
	bool InspectorView() override;

	// TODO All this crapp

	std::string GetParentName() const;
	bool        SetParent(Transform& parent);
	bool        SetParent(Transform& parent, bool worldPositionStays);
	Transform&  Root();
	bool        HasParent() const;
	Transform&  GetParent() const;
	bool        Find(const std::string& nameOfChild, Transform& transform) const;
	bool        FindRecursive(const std::string& nameOfChild, Transform& transform) const;
	Transform&  GetChild(int index) const;

	bool HasChild(SY::UUID id) const;
	bool HasChild(const GameObject& id) const;

	bool                                           HasChildren() const;
	unsigned int                                   GetChildCount() const;
	std::vector<std::reference_wrapper<Transform>> GetAllChildren();
	std::vector<std::reference_wrapper<Transform>> GetAllDirectChildren() const;

	// Will be called from upstairs because gameobjects will also be able to create/destroy children
	bool AddChild(Transform& child);
	bool AddChildren(std::vector<std::reference_wrapper<Transform>>& childs);
	bool RemoveChild(Transform& child);
	void RemoveAllChildren();
	void SetChildrenActive(bool isActive) const;
	void SetChildrenActiveRecursive(bool isActive) const;

	bool IsChildOf(Transform& parent) const;
	void DetachChildren();
	void DetachChildren(int index);
	void DetachChildID(SY::UUID index);
	void DetachChild(const GameObject& index);

	// Detach from parent
	void Detach();

private:
	GameObject              m_Parent;
	std::vector<GameObject> m_Children;
	bool                    IsRecentlyUpdated = false;
#if DoDirtyChecks
    bool IsDirty = true;
#endif
	void           SetDirty(bool arg = true);
	bool           IsDebugGizmoEnabled = false;
	void           MakeClean();
	void           MakeSaneRotation();
	Vector3<float> myPosition{};
	Vector3<float> myRotation{};
	Quaternionf    myQuaternion{};
	Vector3<float> myScale = Vector3f(1, 1, 1);
	;

	Matrix4x4<float>             myTransform{};
	Matrix4x4<float>             myWorldSpaceTransform{};
	DebugDrawer::PrimitiveHandle primitive{};
};

REFL_AUTO(type(Transform, bases<Component>))
