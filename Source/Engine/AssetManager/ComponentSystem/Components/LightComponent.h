#pragma once
#include "Engine/AssetManager/ComponentSystem/Component.h"
#include <Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h>
#include <Tools/Utilities/Color.h >

#include "Engine/AssetManager/Objects/BaseAssets/LightDataBase.h"

enum class eLightType : unsigned int
{
    Directional = 0,
    Point = 1,
    Spot = 2,
    uninitialized = 3
};
class DirectionalLight;
class SpotLight;
class PointLight;

class Texture;
class Light : public Component
{

    friend class GraphicsEngine;
    friend class ShadowRenderer;
    friend class EnvironmentLightPSO;

  public:
    MYLIB_REFLECTABLE();
    Light() = delete;                                             // Create a generic cube
    Light(const SY::UUID anOwnerId, GameObjectManager *aManager); // Create a generic cube
    Light(const SY::UUID anOwnerId, GameObjectManager *aManager, const eLightType type);

    void Init() override;

    eLightType GetType() const;
    void SetType(const eLightType aType);

    bool GetIsShadowCaster() const;
    void SetIsShadowCaster(bool active);

    bool GetIsRendered() const;
    void SetIsRendered(bool isRendered);

    bool GetIsDirty() const;
    void SetIsDirty(bool dirty);

    float GetPower() const;
    void SetPower(float power);

    Color GetColor() const;
    void SetColor(const Color &color);

    void SetPosition(Vector3f position);
    Vector3f GetPosition() const;

    void SetDirection(Vector3f direction);
    Vector3f GetDirection() const;

    void SetRange(float range);
    float GetRange() const;

    void SetInnerAngle(float angle);
    float GetInnerAngle() const;

    void SetOuterAngle(float angle);
    float GetOuterAngle() const;

    std::shared_ptr<Texture> GetShadowMap(int number) const;
    void BindDirectionToTransform(bool active);
    bool GetIsBound() const;

    FrameBuffer GetShadowMapFrameBuffer(const int number = 0) const;
    bool InspectorView() override;

    template <class T> std::shared_ptr<T> GetData();

    void Update() override;
    ~Light() override = default;

    bool boundToTransform = true;
    bool isShadowCaster = false;
    bool isRendered = false;
    Color m_Color;

  private:
    bool isDirty = true;
    void ConformToTransform();
    void RedrawShadowMap();
    void RedrawDirectionMap();
    void RedrawPointMap();
    void RedrawSpotMap();
    Matrix GetLightViewMatrix(int number) const;

    eLightType myLightType;
    std::shared_ptr<DirectionalLight> myDirectionLightData;
    std::shared_ptr<SpotLight> mySpotLightData;
    std::shared_ptr<PointLight> myPointLightData;
    std::shared_ptr<Texture> shadowMap[6];
};

REFL_AUTO(type(Light), field(isRendered), field(isShadowCaster), field(boundToTransform), field(m_Color))

template <> inline std::shared_ptr<DirectionalLight> Light::GetData<DirectionalLight>()
{
    // myDirectionLightData->Color = m_Color.GetRGB();
    return myDirectionLightData;
}

template <> inline std::shared_ptr<SpotLight> Light::GetData<SpotLight>()
{
    return mySpotLightData;
}

template <> inline std::shared_ptr<PointLight> Light::GetData<PointLight>()
{
    // myPointLightData->Color = m_Color.GetRGB();
    return myPointLightData;
}
