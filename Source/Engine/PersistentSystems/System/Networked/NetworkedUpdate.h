#pragma once
#include "Engine/PersistentSystems/System/SystemBase.h"

class NetworkedUpdate : public SystemBase
{
    void Update(float delta) override;
};
