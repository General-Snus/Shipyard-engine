#pragma once
#include "../LinearAlgebra/Matrix4x4.hpp"
#include "../LinearAlgebra/Vectors.hpp" 

class MainCamera
{
public:
	MainCamera(Vector3f aPosition,Vector3f aRotation);
	MainCamera();
	~MainCamera();
	void UpdatePositionVectors();
	void Update(float aTimeDelta);
	Vector4f WoldSpaceToPostProjectionSpace( Vector3f aEntity);
	Matrix GetTransform();
	
	Matrix myClipMatrix;
	Matrix myTransform; 
	Vector3<Vector3f> myDirections;

};
