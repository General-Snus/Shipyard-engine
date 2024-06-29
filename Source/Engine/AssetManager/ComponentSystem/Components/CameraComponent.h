#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Tools/Utilities/Math.hpp>  

#include "Editor/Editor/Windows/Window.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"

struct CameraSettings
{
	float fow = 90.0f;
	float APRatio = 16.0f / 9.0f;;
	float farfield = 1000000.0f;
	float nearField = 0.01f;

	//Explainer: Imgui deals in fractions and projection is more accurate this way.
	Vector2f resolution = {  (float)(Window::Width()), (float)(Window::Height()) };
	bool isOrtho = false;
	bool IsInControll = false;
	float FowInRad() const { return DEG_TO_RAD * fow; };
};

REFL_AUTO(type(CameraSettings)
	, field(fow)
	, field(APRatio)
	, field(farfield)
	, field(nearField)
	, field(isOrtho) 
)

//#define Flashlight
class cCamera : public Component
{
public:
	MYLIB_REFLECTABLE();
	explicit cCamera(const SY::UUID anOwnerId,GameObjectManager* aManager);
	explicit cCamera(const SY::UUID anOwnerId,GameObjectManager* aManager,const CameraSettings& settings);
	~cCamera() override;

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
	bool InspectorView() override;

	CameraSettings mySettings;
private:
	Matrix m_Projection; 
	float cameraSpeed = 25;
};

REFL_AUTO(type(cCamera)
	, field(mySettings) 
)