#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
class Skybox : public Component
{
public:
	Skybox() = delete; // Create a generic cube
	Skybox(const unsigned int anOwnerId); // Create a generic cube 
	Skybox(const unsigned int anOwnerId,const std::filesystem::path aPath);

	void Update() override;
	void Render() override;

private:
	std::shared_ptr<TextureHolder> myCubeMap;
	std::shared_ptr<Material> mySkyboxMaterial;
	std::shared_ptr<Mesh> mySkyboxSphere;
	float m_Radius;
};
