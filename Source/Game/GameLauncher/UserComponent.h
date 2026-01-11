#pragma once
#include "Engine\AssetManager\ComponentSystem\ComponentManager.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Reflection/Reflectable.h>

class UserComponent : public Reflectable<UserComponent>, public Component, SerializableTag
{
public:
	reflectable(UserComponent);
	UserComponent(const SY::UUID anOwnerID, GameObjectManager* aManager) : Component(anOwnerID, aManager)
	{
	}
	void Init() override;
	void Start();

	void Update() override;
	bool InspectorView() override;

	void OnColliderEnter(const SY::UUID aGameObjectID) override;
	void OnColliderExit(const SY::UUID aGameObjectID) override;


	static void Serialize(StreamWriter& writer, UserComponent& d) { writer; d; };
};

REFL_AUTO(type(UserComponent))
