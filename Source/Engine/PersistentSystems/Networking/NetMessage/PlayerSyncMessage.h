#pragma once
#include "NetMessage.h"
#include "Tools/Utilities/LinearAlgebra/Vectors.hpp"
#include "Tools\Utilities\LinearAlgebra\Quaternions.hpp"

struct TransformSyncData
{
	Vector3<float> myPosition{};
	Quaternionf    myQuaternion{};  
	Vector3<float> myScale = Vector3f(1,1,1);  
};

class TransformSyncMessage : public NetMessage
{
  public:
	  TransformSyncMessage()
    {
        myType = eNetMessageType::TransformSyncMessage;
    }

    void SetMessage(const TransformSyncData &someData)
    {
        memcpy(&dataBuffer, &someData, sizeof(someData));
    }

	TransformSyncData ReadMessage() const
    {
		TransformSyncData data;
        memcpy(&data, &dataBuffer, sizeof(data));
        return data;
    }

    constexpr static eNetMessageType type = eNetMessageType::TransformSyncMessage;
  private:
};
