#pragma once 

class cCamera : public Component
{
public: 
	cCamera(const unsigned int anOwnerId); // Create a generic cube
	~cCamera();

	//void UpdatePositionVectors();
	void Update() override;
	void Render() override;

	CU::Vector4<float> WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity); 

private: 
	CU::Matrix4x4<float> myClipMatrix;  
};
