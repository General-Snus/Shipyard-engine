#include "AssetManager.pch.h"
#include "Transform.h"
#include <ThirdParty/CU/Math.hpp>

Transform::Transform(const unsigned int anOwnerId) : Component(anOwnerId) ,isDirty(true)
{
	myTransform = Matrix();
}

Transform::Transform(const unsigned int anOwnerId,const Matrix& aMatrix) : Component(anOwnerId),isDirty(true)
{
	myTransform = aMatrix;
}

void Transform::Update()
{
}

void Transform::Render()
{
}

CU::Matrix4x4<float>& Transform::GetTransform()
{
	return myTransform;
}

const CU::Matrix4x4<float>& Transform::GetTransform() const
{
	return myTransform;
}

Vector3f Transform::GetForward()
{
	return Vector3f(myTransform(3,1),myTransform(3,2),myTransform(3,3)).GetNormalized();
}

Vector3f Transform::GetRight()
{
	return Vector3f(myTransform(1,1),myTransform(1,2),myTransform(1,3)).GetNormalized();
}

Vector3f Transform::GetUp()
{
	return Vector3f(myTransform(2,1),myTransform(2,2),myTransform(2,3)).GetNormalized();
}

void Transform::Move(Vector2f translation)
{
	 myTransform(4,1) += translation.x;
	 myTransform(4,2) += translation.y; 
}

void Transform::Move(Vector3f translation)
{
	myTransform(4,1) += translation.x;
	myTransform(4,2) += translation.y;
	myTransform(4,3) += translation.z;
}

void Transform::SetPosition(Vector2f position)
{
	myTransform(4,1) = position.x;
	myTransform(4,2) = position.y; 
}
void Transform::SetPosition(Vector3f position)
{
	myTransform(4,1) = position.x;
	myTransform(4,2) = position.y;
	myTransform(4,3) = position.z;
}
Vector3f Transform::GetPosition() const
{  
	return {myTransform(4,1),myTransform(4,2),myTransform(4,3)};
}; 

void Transform::Rotate(Vector2f angularRotation)
{
	angularRotation;
	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) * myTransform;

}
void Transform::Rotate(Vector3f angularRotation)
{
	angularRotation;
	myTransform *= Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) *
		Matrix::CreateRotationAroundZ(angularRotation.z * DEG_TO_RAD) * myTransform;
}
void Transform::ApplyTransformation(Matrix transformationMatrix)
{
	transformationMatrix;
	myTransform = transformationMatrix * myTransform;
}

void Transform::SetScale(Vector3f scale)
{
	ApplyTransformation(Matrix::CreateScaleMatrix(scale));
}
void Transform::SetScale(Vector2f scale)
{
	ApplyTransformation(Matrix::CreateScaleMatrix({scale.x,scale.y,1}));
}

void Transform::SetScale(float scale)
{
	ApplyTransformation(Matrix::CreateScaleMatrix({scale,scale,scale}));
}