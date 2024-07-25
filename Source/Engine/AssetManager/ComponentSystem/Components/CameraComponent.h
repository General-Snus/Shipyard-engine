#pragma once
#include "Editor/Editor/Windows/Window.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.h>
#define ENABLE_REFLECTION_FOR(TypeName, ...) \
    REFL_TYPE(TypeName __VA_OPT__(, )) \
    REFL_DETAIL_FOR_EACH(REFL_DETAIL_EX_1_field __VA_OPT__(, ) __VA_ARGS__) \
    REFL_END
class cCamera : public Component
{
  public:
    MYLIB_REFLECTABLE();
    explicit cCamera(const SY::UUID anOwnerId, GameObjectManager *aManager);
    ~cCamera() override;

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
    Vector2f resolution = {(float)(Window::Width()), (float)(Window::Height())};
    bool IsInControll = false;
    Matrix m_Projection;
    float cameraSpeed = 25;
};

REFL_AUTO(type(cCamera), field(fow), field(farfield), field(nearField), field(isOrtho))
