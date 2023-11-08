#pragma once 
#include <Tools/Utilities/Math.hpp>


struct CameraSettings
{
	float fow = PI * (90.0f / 180.0f);;
	float fowmdf = 1.0f / (tanf(fow / 2.0f));;
	float APRatio = 16.0f / 9.0f;;
	float farfield = 15000.0f;
	float nearField = .1f;;
};

class cCamera : public Component
{
public:
	explicit cCamera(const unsigned int anOwnerId); // Create a generic cube
	explicit cCamera(const unsigned int anOwnerId,CameraSettings settings); // Create a generic cube
	~cCamera() override;

	//void UpdatePositionVectors();
	void Update() override;
	void Render() override;
	std::array<Vector4f,4> GetFrustrumCorners() const;
	Vector3f GetPointerDirection(const CU::Vector2<int> position);
	Vector3f GetPointerDirectionNDC(const Vector2f position) const;

	void SetCameraToFrameBuffer();

	CU::Vector4<float> WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity);

private:
	CU::Matrix4x4<float> myClipMatrix;
	CU::Vector2<int> myScreenSize;
	CameraSettings mySettings;
};
