#include "AssetManager.pch.h" 
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
#include <Tools/Utilities/Math.hpp>
#include "../Transform.h"

Transform::Transform(const unsigned int anOwnerId) : Component(anOwnerId),isDirty(true)
{
	myTransform = Matrix();
	myPosition = Vector3f();
	myRotation = Vector3f();
	myScale = Vector3f(1,1,1);
	isDebugGizmoEnabled = false;
}

void Transform::Init()
{
}
/*
Transform::Transform(const unsigned int anOwnerId,const Matrix& aMatrix) : Component(anOwnerId),isDirty(true)
{
	myTransform = aMatrix;
	InitPrimitive();
}
*/
void Transform::Update()
{
	OPTICK_EVENT();
	IsRecentlyUpdated = false;
	if (isDirty)
	{
		MakeClean();
		this->GetGameObject().OnSiblingChanged(&typeid(Transform));
	}
}

void Transform::MakeClean()
{
	IsRecentlyUpdated = true;
	MakeSaneRotation();
	myTransform =
		Matrix::CreateScaleMatrix(myScale) *
		Matrix::CreateRotationAroundX(myRotation.x * DEG_TO_RAD) *
		Matrix::CreateRotationAroundY(myRotation.y * DEG_TO_RAD) *
		Matrix::CreateRotationAroundZ(myRotation.z * DEG_TO_RAD) *
		Matrix::CreateTranslationMatrix(myPosition);
	isDirty = false;

#ifdef _DEBUGDRAW 
	DebugDrawer::Get().SetDebugPrimitiveTransform(primitive,myTransform);
#endif // _DEBUGDRAW 
}

void Transform::Render()
{
	OPTICK_EVENT();
#ifdef _DEBUGDRAW 

#endif // _DEBUGDRAW 
}
const Matrix& Transform::GetTransform()
{
	if (isDirty)
	{
		MakeClean();
		this->GetGameObject().OnSiblingChanged(&typeid(Transform));
	}
	return myTransform;
}

const Quaternionf& Transform::GetQuatF()
{
	return myQuaternion;
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
	isDirty = true;
}
void Transform::Move(Vector3f translation)
{
	myPosition += translation;
	isDirty = true;
}
void Transform::Move(float X,float Y,float Z)
{
	myPosition += {X,Y,Z};
	isDirty = true;
}

void Transform::SetPosition(Vector2f position)
{
	//myTransform(4,1) = position.x;
	//myTransform(4,2) = position.y;
	myPosition.x = position.x;
	myPosition.y = position.y;
	isDirty = true;
}
void Transform::SetPosition(Vector3f position)
{
	myPosition = position;
	isDirty = true;
}
void Transform::SetPosition(float X,float Y,float Z)
{
	myPosition = { X,Y,Z };
	isDirty = true;
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
	isDirty = true;
}
void Transform::Rotate(Vector2f angularRotation)
{
	myRotation.x += angularRotation.x;
	myRotation.y += angularRotation.y;
	isDirty = true;

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
	isDirty = true;
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
	isDirty = true;
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
	return isDirty;
}

void Transform::SetGizmo(bool enabled)
{
	isDebugGizmoEnabled = enabled;
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

void Transform::SetRotation(float X,float Y,float Z)
{
	myRotation = { X,Y,Z };

	myQuaternion.SetEulerAngles(myRotation);
	isDirty = true;
}

void Transform::SetRotation(Vector2f angularRotation)
{
	myRotation.x = angularRotation.x;
	myRotation.y = angularRotation.y;
	myQuaternion.SetEulerAngles(myRotation);
	isDirty = true;
}

void Transform::SetRotation(Vector3f angularRotation)
{
	myRotation = angularRotation;
	myQuaternion.SetEulerAngles(myRotation);
	isDirty = true;
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
	isDirty = true;
}

void Transform::SetScale(Vector2f scale)
{
	myScale.x = scale.x;
	myScale.y = scale.y;
	isDirty = true;
}

void Transform::SetScale(float scale)
{
	myScale.x = scale;
	myScale.y = scale;
	myScale.z = scale;
	isDirty = true;
	//ApplyTransformation(Matrix::CreateScaleMatrix({scale,scale,scale}));
}