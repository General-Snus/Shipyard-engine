#pragma once
 
#include <AssetManager/AssetManager.pch.h>
#include <GraphicsEngine/DebugDrawer/DebugDrawer.h>
typedef CU::Vector3<float> Vector3f;
typedef CU::Matrix4x4<float> Matrix;

class Transform : public Component
{
public:
	Transform() = delete; // Create a generic cube
	Transform(const unsigned int anOwnerId); // Create a generic cube 
	Transform(const unsigned int anOwnerId,const Matrix& aMatrix);

	void Update() override;
	void Render() override;
	Matrix& GetTransform();
	const Matrix& GetTransform() const;



	// TODO
	//SET A UNIVERSAL METER
	Vector3f GetForward();
	Vector3f GetRight();
	Vector3f GetUp();

	void Move(Vector2f translation);
	void Move(Vector3f translation);

	void SetPosition(Vector2f position);
	void SetPosition(Vector3f position);
	Vector3f GetPosition() const;   
	 
	void Rotate(Vector2f angularRotation,bool worldSpace);
	void Rotate(Vector3f angularRotation,bool worldSpace);
	void ApplyTransformation(Matrix transformationMatrix);
	 
	void SetScale(float scale);
	void SetScale(Vector2f scale);
	void SetScale(Vector3f scale); 

	~Transform() = default;
	void SetGizmo(bool enabled);
	void InitPrimitive();
private:
	bool isDirty;
	bool isDebugGizmoEnabled;
	CU::Matrix4x4<float> myTransform;
	CU::Matrix4x4<float> myWorldSpaceTransform;
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

