#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/ParticleEmitter.h> 

class ParticleSystem : public Reflectable<ParticleSystem>, public Component
{
public:
	reflectable(ParticleSystem);
	ParticleSystem() = delete; // Create a generic cube
	ParticleSystem(const unsigned int anOwnerId); // Create a generic cube 
	ParticleSystem(const unsigned int anOwnerId, const std::filesystem::path& aFilePath);

	void AddEmitter(const ParticleEmitterTemplate& aTemplate);

	void Update() override;
	void Draw(); // Draw is a custom render function that is called at will
private:
	std::vector<std::shared_ptr<ParticleEmitter>> myEmitters;

};

REFL_AUTO(type(ParticleSystem))
