#pragma once

#include <Engine/AssetManager/ComponentSystem/Component.h>

#include <filesystem>
#include <memory>
#include <unordered_set>
#include "Engine/PersistentSystems/Networking/NetworkStructs.h"
#include "Tools/Utilities/LinearAlgebra/Quaternions.hpp"

//Existance sync
class NetworkObject : public Component
{
  public:
    ReflectableTypeRegistration();
    NetworkObject() = delete;
    NetworkObject(const SY::UUID anOwnerId, GameObjectManager *aManager);
    NetworkObject(const SY::UUID anOwnerId, GameObjectManager *aManager,NetworkedId id);
	void Init() override;
	void Destroy() override;
    bool InspectorView() override;

    NetworkedId GetServerID() const
    {
        return uniqueNetId;
    }

    void DisperseNetMessage(const NetMessage& netMessageForIndividualobject);

  private:
      NetworkedId uniqueNetId;
};
REFL_AUTO(type(NetworkObject))
//TODO SHOULD HAVE NETWORK COMPONENT PARENT TO HIDE A BIT SHITTY THINGS

//Transform sync
class NetworkTransform : public Component
{
public:
    ReflectableTypeRegistration();
    NetworkTransform() = delete;
    NetworkTransform(const SY::UUID anOwnerId,GameObjectManager* aManager);
	bool InspectorView() override;
	void Init() override;

	std::chrono::time_point<std::chrono::high_resolution_clock> updatePoint;
	Vector3f myPosition; // Update from server
	Quaternionf    myQuaternion; // Update from server
	Vector3f myScale = Vector3f(1,1,1); // Update from server

	Vector3f translationInterpolation; // Update on client whenever new message
	Quaternionf    rotationInterpolation; // Update on client whenever new message

	NetworkedId GetServerID() const
	{
		return uniqueNetId;
	}
private:
	NetworkedId uniqueNetId; 
};
REFL_AUTO(type(NetworkTransform),field(myPosition),field(translationInterpolation))
