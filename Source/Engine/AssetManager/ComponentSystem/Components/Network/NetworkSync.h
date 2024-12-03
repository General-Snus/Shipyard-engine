#pragma once

#include <Engine/AssetManager/ComponentSystem/Component.h>

#include <filesystem>
#include <memory>
#include <unordered_set>
class NetworkSync : public Component
{
  public:
    ReflectableTypeRegistration();
    NetworkSync() = delete; // Create a generic cube
    NetworkSync(const SY::UUID anOwnerId, GameObjectManager *aManager);
    bool InspectorView() override;

    SY::UUID GetServerID()
    {
        return m_ServerID;
    }

  private:
    SY::UUID m_ServerID;
};
REFL_AUTO(type(NetworkSync))
