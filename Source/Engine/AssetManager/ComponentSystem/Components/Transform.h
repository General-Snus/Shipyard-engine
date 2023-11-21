#pragma once 
#include <Engine/GraphicsEngine/DebugDrawer/DebugDrawer.h>
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>

class Transform : public Component
{
public:
	Transform() = delete; // Create a generic cube
	Transform(const unsigned int anOwnerId); // Create a generic cube 
	//Transform(const unsigned int anOwnerId,const Matrix& aMatrix);

	void Update() override;
	void Render() override;
	// Matrix& GetTransform();
	const Matrix& GetTransform() const;



	// TODO
	//SET A UNIVERSAL METER
	Vector3f GetForward();
	Vector3f GetRight();
	Vector3f GetUp();

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
	//void ApplyTransformation(Matrix transformationMatrix);
	 
	void SetScale(float scale);
	void SetScale(Vector2f scale);
	void SetScale(Vector3f scale); 

	bool GetIsDirty() const;

	~Transform() = default;
	void SetGizmo(bool enabled);
	void InitPrimitive();
private:
	bool isDirty;
	bool isDebugGizmoEnabled;
	void MakeSaneRotation();
	Vector3<float> myPosition;
	Vector3<float> myRotation;
	Vector3<float> myScale;



	Matrix4x4<float> myTransform;
	Matrix4x4<float> myWorldSpaceTransform;
	Debug::DebugPrimitive primitive;
	
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

