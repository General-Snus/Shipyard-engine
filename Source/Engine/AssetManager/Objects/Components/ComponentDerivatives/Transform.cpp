#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include "Transform.h"
#include <Tools/Utilities/Math.hpp>

Transform::Transform(const unsigned int anOwnerId) : Component(anOwnerId),isDirty(true)
{
	myTransform = Matrix();
	myPosition = Vector3f();
	myRotation = Vector3f();
	myScale = Vector3f(1,1,1);


	InitPrimitive();
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
	if(isDirty)
	{
		MakeSaneRotation();
		myTransform =
			Matrix::CreateScaleMatrix(myScale) *
			Matrix::CreateRotationAroundX(myRotation.x * DEG_TO_RAD) *
			Matrix::CreateRotationAroundY(myRotation.y * DEG_TO_RAD) *
			Matrix::CreateRotationAroundZ(myRotation.z * DEG_TO_RAD) *
			Matrix::CreateTranslationMatrix(myPosition); 
		isDirty = false;
	}
}

void Transform::Render()
{ 
#ifndef _DEBUGDRAW 
	if(primitive.NumVertices != 0 && primitive.NumIndices != 0)
	{
		GraphicsEngine::Get().OverlayCommands<GfxCmd_DrawDebugPrimitive>(primitive,myTransform);
	}
#endif // _DEBUGDRAW 
} 
//CU::Matrix4x4<float>& Transform::GetTransform()
//{
//	return myTransform;
//} 
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
	//myTransform(4,1) += translation.x;
	//myTransform(4,2) += translation.y;
	myPosition.x += translation.x;
	myPosition.y += translation.y;
	isDirty = true;
}

void Transform::Move(Vector3f translation)
{
	//myTransform(4,1) += translation.x;
	//myTransform(4,2) += translation.y;
	//myTransform(4,3) += translation.z;
	myPosition += translation;
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
void Transform::SetPosition(float X, float Y,float Z)
{ 
	myPosition = {X,Y,Z};
	isDirty = true;
}

Vector3f Transform::GetPosition() const
{
	return {myTransform(4,1),myTransform(4,2),myTransform(4,3)};
};

void Transform::Rotate(float X, float Y, float Z)
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

void Transform::MakeSaneRotation()
{
	for(int i = 0; i < 3; i++)
	{
		if(std::abs(myRotation[i]) > 360)
		{
			myRotation[i] = std::fmodf(myRotation[i],360);
		}
	}
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

bool Transform::GetIsDirty() const
{
	return isDirty;
}

void Transform::SetGizmo(bool enabled)
{
	isDebugGizmoEnabled = enabled;
	if(enabled)
	{
		InitPrimitive();
	}
	else
	{
		primitive = Debug::DebugPrimitive();
	}
}

void Transform::InitPrimitive()
{ 
	float size = 10.0f;
	float thicc = 15.0f;
	std::vector<Debug::DebugVertex> myVertex;
	std::vector<unsigned int> myIndices;
	//X 
	myVertex.push_back(
		Debug::DebugVertex({0,0,0},{1,0,0},thicc));
	myVertex.push_back(
		Debug::DebugVertex({size,0,0},{1,0,0},thicc));
	myIndices.push_back(0);
	myIndices.push_back(1);

	//Y
	myVertex.push_back(
		Debug::DebugVertex({0,0,0},{0,1,0},thicc));
	myVertex.push_back(
		Debug::DebugVertex({0,size,0},{0,1,0},thicc));
	myIndices.push_back(2);
	myIndices.push_back(3);

	//Z
	myVertex.push_back(
		Debug::DebugVertex({0,0,0},{0,0,1},thicc));
	myVertex.push_back(
		Debug::DebugVertex({0,0,size},{0,0,1},thicc));
	myIndices.push_back(4);
	myIndices.push_back(5); 
	
	primitive = Debug::DebugPrimitive(myVertex,myIndices); 
}

void Transform::SetScale(Vector2f scale)
{
	myScale.x = scale.x;
	myScale.y = scale.y;
	isDirty = true;
	//ApplyTransformation(Matrix::CreateScaleMatrix({scale.x,scale.y,1}));
}

void Transform::SetScale(float scale)
{
	myScale.x = scale;
	myScale.y = scale;
	myScale.z = scale;
	isDirty = true;
	//ApplyTransformation(Matrix::CreateScaleMatrix({scale,scale,scale}));
}