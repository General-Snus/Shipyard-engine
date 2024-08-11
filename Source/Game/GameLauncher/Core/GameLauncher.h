#pragma once
#include "Engine/AssetManager/ComponentSystem/GameObject.h"

#include <Tools/Utilities/System/Event.h>
class GameLauncher
{
  public:
    GameLauncher() = default;
    void Init();
    void Start();
    void Update(float delta);

    void LocalFunction();

  private:
    Event m_CustomKeyCallback;
};
