#pragma once
#include "Engine/AssetManager/Reflection/Reflectable.h"
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>
#include <typeinfo>

class Collider;
class Transform;
class physx::PxShape;
class physx::PxRigidStatic;

class cPhysXStaticBody : public Reflectable<cPhysXStaticBody>, public Component, SerializableTag
{
public:
	reflectable(cPhysXStaticBody);
	defaultComponentInspector();
	cPhysXStaticBody(const SY::UUID anOwnerId, GameObjectManager* aManager);

	void Init() override;
	void UpdateFromCollider();
	void makeShape(const Collider* collider, const Transform& transform);
	void updateShape(const Collider* collider, const Transform& transform);
	void Update() override;
	void Destroy() override;
	void OnSiblingChanged(const std::type_info* SourceClass) override;

private:
	physx::PxRigidStatic* data;
	physx::PxShape* shape = {}; //TODO: FIX THIS
};

REFL_AUTO(type(cPhysXStaticBody))
