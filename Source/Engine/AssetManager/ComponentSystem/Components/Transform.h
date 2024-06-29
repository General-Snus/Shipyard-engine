#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>



//LEFTHANDED X RIGHT Y UP Z FORWARD AS GOD INTENDED
class Transform : public Component
{
public:
	MYLIB_REFLECTABLE();
	Transform() = delete;
	Transform(const SY::UUID anOwnerId,GameObjectManager* aManager);

	void Init() override;
	void Update() override;
	void Render() override;
	//Be ware, the matrix is built by the transform, changes wont carry but you can use this to avoid making copies of the orignal matrix
	Matrix& GetMutableTransform();
	const Matrix& GetTransform();
	//not dirty checked
	const Matrix& GetRawTransform() const;
	const Quaternionf& GetQuatF();
	void SetQuatF(const Quaternionf&);


	// TODO
	//SET A UNIVERSAL METER
	Vector3f GetForward() const;
	Vector3f GetRight() const;
	Vector3f GetUp() const;

	void Move(Vector2f translation);
	void Move(Vector3f translation);
	void Move(float X,float Y,float Z);

	void SetPosition(Vector2f position);
	void SetPosition(Vector3f position);
	void SetPosition(float X,float Y,float Z);
	Vector3f GetPosition() const;


	void Rotate(float X,float Y,float Z);
	void Rotate(Vector2f angularRotation);
	void Rotate(Vector3f angularRotation);
	void SetRotation(float X,float Y,float Z);
	void SetRotation(Vector2f angularRotation);
	void SetRotation(Vector3f angularRotation);
	void LookAt(Vector3f target);

	//"You will eventually regret any use of Euler angles."
	//John Carmack
	Vector3f GetRotation() const;
	//Not mathematicly sound
	Vector3f VectorToEulerAngles(Vector3f input) const;

	void SetScale(float X,float Y,float Z);

	void SetScale(float scale);
	void SetScale(Vector2f scale);
	void SetScale(Vector3f scale);
	Vector3f GetScale() const;

	//Meaning last frame
	bool GetIsRecentlyUpdated() const;
	bool GetIsDirty() const;

	~Transform() override = default;
	void SetGizmo(bool enabled);
	void InitPrimitive();
	bool InspectorView() override;







	//TODO All this crapp


	std::string& GetParentName()  const;
	void SetParent(Transform& parent);
	void SetParent(Transform& parent,bool worldPositionStays);
	Transform& Root() const;
	Transform& GetParent() const;
	Transform& Find(const std::string& nameOfChild) const;
	Transform& FindRecursive(const std::string& nameOfChild) const;
	Transform& GetChild(int index) const;
	bool HasChildren() const;
	unsigned int GetChildCount() const;
	std::vector<Transform&> GetAllChildren() const;
	std::vector<Transform&> GetAllDirectChildren() const;

	//Will be called from upstairs because gameobjects will also be able to create/destroy children
	void AddChild(Transform& child);
	void AddChildren(std::vector<Transform&>& childs);
	void RemoveChild(Transform& child);
	void SetChildrenActive(bool isActive) const;
	void SetChildrenActiveRecursive(bool isActive) const;


	bool IsChildOf(Transform& parent) const;
	void DetachChildren() const;
	void DetachChildren(int index) const;

	//Detach from parent
	void Detach();

private:
	GameObject m_Parent;
	std::vector<GameObject> m_Children;

	bool IsRecentlyUpdated = false;
	bool IsDirty = true;
	bool IsDebugGizmoEnabled = false;
	void MakeClean();
	void MakeSaneRotation();

	Vector3<float> myPosition{};
	Vector3<float> myRotation{};
	Quaternionf myQuaternion{};
	Vector3<float> myScale = Vector3f(1,1,1);;

	Matrix4x4<float> myTransform{};
	Matrix4x4<float> myWorldSpaceTransform{};
	DebugDrawer::PrimitiveHandle primitive{};
};


REFL_AUTO(type(Transform,bases<Component>))
