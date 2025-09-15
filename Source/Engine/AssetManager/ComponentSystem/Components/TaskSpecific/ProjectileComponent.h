#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class ProjectileComponent : public Reflectable<ProjectileComponent>, public Component
{
public:
    reflectable(ProjectileComponent);
    ProjectileComponent(const SY::UUID anOwnerID, GameObjectManager *aManager);
    ~ProjectileComponent() override = default;
    void Init() override;
    void Update() override;

    void InitWithValues(float aLifetime, GameObject aCreator);

    void OnColliderEnter(const SY::UUID aGameObjectID) override;

private:
    float lifetime;
    GameObject Creator;
};

REFL_AUTO(type(ProjectileComponent))
