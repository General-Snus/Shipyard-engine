#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class UserComponent : public Component
{

  public:
    MYLIB_REFLECTABLE();

    void Init() override;
    void Start();

    void Update() override;
    bool InspectorView() override;

    void OnColliderEnter(const SY::UUID aGameObjectID) override;
    void OnColliderExit(const SY::UUID aGameObjectID) override;
};

REFL_AUTO(type(UserComponent))
