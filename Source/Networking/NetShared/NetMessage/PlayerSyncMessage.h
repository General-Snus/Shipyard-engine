#pragma once
#include "NetMessage.h"
#include "Tools/Utilities/LinearAlgebra/Vectors.hpp"
struct PlayerSyncData
{
    unsigned long long id;
    Vector3f position;
    Vector3f rotation;
    Vector3f scale;
    Vector2f blendPosition;
};

class PlayerSyncMessage : public NetMessage
{
  public:
    PlayerSyncMessage()
    {
        myType = eNetMessageType::PlayerSync;
    }

    void SetMessage(const PlayerSyncData &someData)
    {
        memcpy(&myBuffer, &someData, sizeof(someData));
    }

    PlayerSyncData ReadMessage()
    {
        PlayerSyncData data;
        memcpy(&data, &myBuffer, sizeof(data));
        return data;
    }

  private:
};
