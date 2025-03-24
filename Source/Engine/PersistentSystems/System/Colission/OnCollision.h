#pragma once
#include "Engine/AssetManager/ComponentSystem/UUID.h"
#include "Engine/PersistentSystems/System/SystemBase.h"
#include <unordered_map>
#include <unordered_set>

class CollisionChecks : public SystemBase
{
  public:
    void Update(float delta) override;
    void RemoveCollisions(const SY::UUID &anOwnerID);

  private:
    void CheckColliders();
    std::unordered_map<SY::UUID, std::unordered_map<SY::UUID, bool>> activeCollisions;
};
