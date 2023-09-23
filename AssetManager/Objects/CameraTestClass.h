#pragma once 
	
class MainCamera
{
public:
	MainCamera(CommonUtilities::Vector3<float> aPosition, CommonUtilities::Vector3<float> aRotation);
	MainCamera();
	~MainCamera();
	void UpdatePositionVectors();
	void Update(float aTimeDelta);
	CU::Vector4<float> WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity);
	CU::Matrix4x4<float>& GetTransform();
	Vector3f GetPosition() const;
	CU::Matrix4x4<float> myClipMatrix;
	CU::Matrix4x4<float> myTransform;
	CU::Vector3<CommonUtilities::Vector3<float>> myDirections;

};
