#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Tools/Utilities/Math.hpp>  
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"

struct CameraSettings
{
	float fow = PI * (90.0f / 180.0f);;
	float APRatio = 16.0f / 9.0f;;
	float farfield = 1000000.0f;
	float nearField = 0.01f;
	bool isOrtho = false;
	bool IsInControll = false;
};

//#define Flashlight
class cCamera : public Component
{
public:
	MYLIB_REFLECTABLE();
	explicit cCamera(const unsigned int anOwnerId); // Create a generic cube
	explicit cCamera(const unsigned int anOwnerId,const CameraSettings& settings); // Create a generic cube
	~cCamera() override;

	//void UpdatePositionVectors();
	void Update() override;
	void Render() override;

	void SetSettings(const CameraSettings& settings);
	CameraSettings& GetSettings() { return mySettings; };

	std::array<Vector4f,4> GetFrustrumCorners() const;
	Vector3f GetPointerDirection(const Vector2<int> position);
	Vector3f GetPointerDirectionNDC(const Vector2<int> position) const;

	Matrix GetProjection() { return m_Projection; };
	FrameBuffer GetFrameBuffer();
	Vector4f WoldSpaceToPostProjectionSpace(Vector3f aEntity);
	void InspectorView() override;

private:
	Matrix m_Projection;
	Vector2ui myScreenSize;
	float cameraSpeed = 10;
	CameraSettings mySettings;
};

REFL_AUTO(type(cCamera))