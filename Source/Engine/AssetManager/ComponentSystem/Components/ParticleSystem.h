#pragma once
#include <Engine/AssetManager/ComponentSystem/Component.h>
#include <Engine/AssetManager/Objects/BaseAssets/ParticleEmitter.h> 

class ParticleSystem : public Component
{
public:
	MYLIB_REFLECTABLE();
	ParticleSystem() = delete; // Create a generic cube
	ParticleSystem(const unsigned int anOwnerId); // Create a generic cube 
	ParticleSystem(const unsigned int anOwnerId,const std::filesystem::path& aFilePath);

	void AddEmitter(const ParticleEmitterTemplate& aTemplate);

	void Update() override;
	void Draw(); // Draw is a custom render function that is called at will
private:
	std::vector<std::shared_ptr<ParticleEmitter>> myEmitters;

};

REFL_AUTO(type(ParticleSystem))
