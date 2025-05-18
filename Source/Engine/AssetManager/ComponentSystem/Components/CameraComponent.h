#pragma once
#include "Editor/Editor/Windows/Window.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

enum class DebugFilter {
	NoFilter = 0,
	WorldPosition,
	VertetexWorldNormal,
	AlbedoMap,
	NormalMap,
	WorldPixelNormal,
	AmbientOcclusion,
	Roughness,
	Metallic,
	Emission,
	Depth,
	DirectionLight,
	AmbientLight,
	PointLight,
	SpotLight,
	ShadowMap,
	TexCoord0,
	TexCoord1,
	TexCoord2,
	TexCoord3,
	TexCoord4,
	TexCoord5,
	count
};

class Camera : public Component {
public:
	ReflectableTypeRegistration();
	explicit Camera(const SY::UUID anOwnerId,GameObjectManager* aManager);

	void Update() override;
	void EditorCameraControlls();
	bool InspectorView() override;

	std::array<Vector4f,4> GetFrustrumCorners() const;
	Vector3f GetPointerDirection(const Vector2f position);
	Vector3f GetPointerDirectionNDC(const Vector2f position) const;
	Vector4f WoldSpaceToPostProjectionSpace(Vector3f aEntity);

	inline void IsInControl(bool aIsInControl) {
		IsInControll = aIsInControl;
	};

	inline Matrix GetProjection() {
		return m_Projection;
	};
	const Matrix& ViewMatrix();

	FrameBuffer GetFrameBuffer();
	Transform& LocalTransform();
	const Transform& LocalTransform() const;
	void UpdateProjection();

	float fow = 90.0f;
	float farfield = 1000000.0f;
	float nearField = 0.01f;
	bool isOrtho = false;
	Vector2f orthoRect{};
	DebugFilter filter = DebugFilter::NoFilter;

	inline float FowInRad() const {
		return Math::DEG_TO_RAD * fow;
	};

	inline float APRatio() const {
		auto res = resolution;
		res.x = std::max(resolution.x, 1.0f);
		res.y = std::max(resolution.y, 1.0f);
		return res.x / res.y;
	};

	inline void SetResolution(Vector2f aResolution) {
		resolution = aResolution;
	};

	inline void SetResolution(Vector2ui aResolution)
	{
		resolution.x = (float)aResolution.x;
		resolution.y = (float)aResolution.y;
	};


private:
	Transform localTransform;
	Vector2f resolution = {(float)(WindowInstance.Width()), (float)(WindowInstance.Height())};
	bool IsInControll = false;
	Matrix m_Projection;
	Matrix m_CachedViewMatrix;
	float cameraSpeed = 25;
};
//TODO MAKE PROPERTY LIKE REFLECTION INSTEAD OF PURE VARIABLE
REFL_AUTO(type(Camera),field(fow),field(farfield),field(nearField),field(isOrtho))
