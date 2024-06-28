#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
class Skybox : public Component
{
public:
	MYLIB_REFLECTABLE();
	Skybox() = delete;
	Skybox(const SY::UUID anOwnerId,GameObjectManager* aManager);
	Skybox(const SY::UUID anOwnerId,GameObjectManager* aManager,const std::filesystem::path& aPath);

	void Update() override;
	bool InspectorView()override;
	void Render() override;

private:
	std::shared_ptr<TextureHolder> myCubeMap;
	std::shared_ptr<Material> mySkyboxMaterial;
	std::shared_ptr<Mesh> mySkyboxSphere;
	float m_Radius;
};

REFL_AUTO(type(Skybox))