#pragma once

#include "CommonUtills/Matrix4x4.hpp"
#include "CommonUtills/Vectors.hpp"

 

class MainCamera
{
public:
	MainCamera(CommonUtilities::Vector3<float> aPosition,CommonUtilities::Vector3<float> aRotation);
	MainCamera();
	~MainCamera();
	void UpdatePositionVectors();
	void Update(float aTimeDelta);
	CommonUtilities::Vector4<float> WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity);
	CommonUtilities::Matrix4x4<float> GetTransform();
	 
	CommonUtilities::Matrix4x4<float> myClipMatrix;
	CommonUtilities::Matrix4x4<float> myTransform; 
	CommonUtilities::Vector3<CommonUtilities::Vector3<float>> myDirections;

};
