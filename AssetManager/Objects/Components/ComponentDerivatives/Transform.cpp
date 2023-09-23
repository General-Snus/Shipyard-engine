#include "AssetManager.pch.h"
#include "Transform.h"
#include <ThirdParty/CU/Math.hpp>
Transform::Transform(const unsigned int anOwnerId) : Component(anOwnerId)
{
}

Transform::Transform(const unsigned int anOwnerId,const Matrix& aMatrix) : Component(anOwnerId)
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
	return Vector3f();
}

Vector3f Transform::GetRight()
{
	return Vector3f();
}

Vector3f Transform::GetUp()
{
	return Vector3f();
}

void Transform::SetPosition(Vector2f position)
{
	position;
}
void Transform::SetPosition(Vector3f position)
{
	position;
}
Vector3f Transform::GetPosition() const
{
	return Vector3f();
}

void Transform::Rotate(Vector2f angularRotation)
{
	angularRotation;
	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD);

}
void Transform::Rotate(Vector3f angularRotation)
{
	angularRotation;
	myTransform = Matrix::CreateRotationAroundX(angularRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(angularRotation.y * DEG_TO_RAD) *
		Matrix::CreateRotationAroundZ(angularRotation.z * DEG_TO_RAD);
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