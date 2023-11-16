#pragma once 
#include <Tools/Utilities/Math.hpp>


struct CameraSettings
{
	float fow = PI * (90.0f / 180.0f);; 
	float APRatio = 16.0f / 9.0f;;
	float farfield = 1000000.0f;
	//float farfield = std::numeric_limits<float>::infinity();
	float nearField = 1.0f;
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
	Vector3f GetPointerDirection(const Vector2<int> position);
	Vector3f GetPointerDirectionNDC(const Vector2<int> position) const;

	void SetCameraToFrameBuffer();

	Vector4f WoldSpaceToPostProjectionSpace(Vector3f aEntity);

private:
	Matrix myClipMatrix;
	Vector2<int> myScreenSize;
	float cameraSpeed = 10;
	CameraSettings mySettings;
};
