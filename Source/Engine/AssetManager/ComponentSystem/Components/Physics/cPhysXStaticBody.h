#pragma once 
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/PersistentSystems/Physics/PhysXInterpeter.h>


class cPhysXStaticBody : public Component
{
public:
	cPhysXStaticBody(const SY::UUID anOwnerID);

	void Init() override;
	void UpdateFromCollider();
	void Update() override;
	void Render() override;
	void Destroy() override;
	void OnSiblingChanged(const std::type_info* SourceClass) override;
private:
	physx::PxRigidStatic* data;
};

REFL_AUTO(type(cPhysXStaticBody))