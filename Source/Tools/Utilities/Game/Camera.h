#pragma once
#include "../LinearAlgebra/Vectors.hpp"
#include "Tools/Utilities/LinearAlgebra/Matrix4x4.h"

class MainCamera
{
public:
	MainCamera(Vector3f aPosition,Vector3f aRotation);
	MainCamera();
	~MainCamera();
	void     UpdatePositionVectors();
	void     Update(float aTimeDelta);
	Vector4f WoldSpaceToPostProjectionSpace(Vector3f aEntity) const;
	Matrix   GetTransform();

	Matrix myClipMatrix;
	Matrix myTransform;
	Vector3<Vector3f> myDirections;

};
