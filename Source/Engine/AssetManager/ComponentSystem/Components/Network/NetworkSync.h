#pragma once

#include <Engine/AssetManager/ComponentSystem/Component.h>

#include <filesystem>
#include <memory>
#include <unordered_set>
#include "Engine/PersistentSystems/Networking/NetworkStructs.h" 
#include "Tools/Utilities/LinearAlgebra/Quaternions.hpp"
#include "Tools/Utilities/Input/Mapper.hpp"

//Existance sync
class NetworkRunner;
class NetworkObject : public Component
{
public:
	ReflectableTypeRegistration();
	NetworkObject() = delete;
	NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager);
	NetworkObject(const SY::UUID anOwnerId, GameObjectManager* aManager, NetworkedId id);
	void Init() override;
	void Destroy() override;
	bool InspectorView() override;

	NetworkedId GetServerID() const
	{
		return uniqueNetId;
	}
	float SyncFrequencyInverse() const
	{
		return 1 / syncFrequency;
	}

	ServerTimePoint GetLastSyncTime() const
	{
		return clientUpdateTimePoint;
	}

	bool ShouldSync(const NetworkRunner& runner) const;
	bool ShouldSync(const NetworkRunner& runner, float customSyncTime) const;
	void Synced(const ServerTimePoint& time);

	void DisperseNetMessage(const NetMessage& netMessageForIndividualobject);

	float syncFrequency = 20.0f;
protected:
	ServerTimePoint clientUpdateTimePoint;
	NetworkedId uniqueNetId;
};
REFL_AUTO(type(NetworkObject))
//TODO SHOULD HAVE NETWORK COMPONENT PARENT TO HIDE A BIT SHITTY THINGS

//Transform sync
class NetworkTransform : public NetworkObject
{
public:
	ReflectableTypeRegistration();
	NetworkTransform() = delete;
	NetworkTransform(const SY::UUID anOwnerId, GameObjectManager* aManager);
	bool InspectorView() override;
	void Init() override;

	Vector3f myPosition; // Update from server
	Quaternionf    myQuaternion; // Update from server
	Vector3f myScale = Vector3f(1, 1, 1); // Update from server

	Vector3f translationInterpolation; // Update on client whenever new message
	Quaternionf    rotationInterpolation; // Update on client whenever new message
};
REFL_AUTO(type(NetworkTransform), field(myPosition), field(translationInterpolation))


class NetworkInputListener : public NetworkObject
{
public:
	ReflectableTypeRegistration();
	NetworkInputListener() = delete;
	NetworkInputListener(const SY::UUID anOwnerId, GameObjectManager* aManager);
	bool InspectorView() override;
	void Init() override;

	void AddKeyFunc(Keys key, Action action, MapperFunction func);
};
REFL_AUTO(type(NetworkInputListener))
