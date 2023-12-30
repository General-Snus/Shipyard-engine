#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>

class ProjectileComponent : public Component
{
public:
	ProjectileComponent(const SY::UUID anOwnerID);
	~ProjectileComponent() = default;
	void Init() override;
	void Update() override;


	void InitWithValues(float aLifetime,GameObject aCreator);

	void CollidedWith(const SY::UUID aGameObjectID) override;
private:
	float lifetime;
	GameObject Creator;
};

