#pragma once
#include "Editor/Editor/Windows/Window.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>

class Camera : public Component
{
  public:
    MYLIB_REFLECTABLE();
    explicit Camera(const SY::UUID anOwnerId, GameObjectManager *aManager);
    ~Camera() override;

    void Update() override;
    void Render() override;
    bool InspectorView() override;

    std::array<Vector4f, 4> GetFrustrumCorners() const;
    Vector3f GetPointerDirection(const Vector2<int> position);
    Vector3f GetPointerDirectionNDC(const Vector2<int> position) const;
    Vector4f WoldSpaceToPostProjectionSpace(Vector3f aEntity);

    inline void IsInControl(bool aIsInControl)
    {
        IsInControll = aIsInControl;
    };

    inline Matrix GetProjection()
    {
        return m_Projection;
    };
    FrameBuffer GetFrameBuffer();

    float fow = 90.0f;
    float farfield = 1000000.0f;
    float nearField = 0.01f;
    bool isOrtho = false;

    inline float FowInRad() const
    {
        return DEG_TO_RAD * fow;
    };

    inline float APRatio() const
    {
        return resolution.x / resolution.y;
    };

    inline void SetResolution(Vector2f aResolution)
    {
        resolution = aResolution;
    };

  private:
    Vector2f resolution = {(float)(WindowInstance.Width()), (float)(WindowInstance.Height())};
    bool IsInControll = false;
    Matrix m_Projection;
    float cameraSpeed = 25;
};

REFL_AUTO(type(Camera), field(fow), field(farfield), field(nearField), field(isOrtho))
