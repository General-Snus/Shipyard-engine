#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>



//LEFTHANDED X RIGHT Y UP Z FORWARD AS GOD INTENDED
class Transform : public Component
{
public:
	MYLIB_REFLECTABLE();
	Transform() = delete; // Create a generic cube
	Transform(const unsigned int anOwnerId); // Create a generic cube 
	//Transform(const unsigned int anOwnerId,const Matrix& aMatrix);

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

	void InspectorView() override;
private:

	bool IsRecentlyUpdated;
	bool isDirty;
	bool isDebugGizmoEnabled;
	void MakeClean();
	void MakeSaneRotation();
	Vector3<float> myPosition;
	Vector3<float> myRotation;
	Quaternionf myQuaternion;
	Vector3<float> myScale;

	Matrix4x4<float> myTransform;
	Matrix4x4<float> myWorldSpaceTransform;
	DebugDrawer::PrimitiveHandle primitive;

	/*

*  ALL IN RADIANS FOR NOW, HEADER FOR CONVERTIONS CONST

void LookAt
void GetEulerAxis
void SetRotatation
void

void SetPosition
GetPosition


GetScale
//Rotate aroundpoint
void SetParent(Transform)
GetParent
void SetChild(Transform)
GetChild
GetChilds
GetLocalToWorldMatrix
GetWorldToLocalMatrix

void ChangePrecision(base10)
void ChangeMeterScale(set);

*/

};


REFL_AUTO(type(Transform,bases<Component>))
