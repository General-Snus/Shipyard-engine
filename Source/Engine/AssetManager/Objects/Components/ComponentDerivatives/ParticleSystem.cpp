#include "AssetManager.pch.h"
#include "ParticleSystem.h"
#include <Tools/Utilities/Game/Timer.h>

ParticleSystem::ParticleSystem(const unsigned int anOwnerId) : Component(anOwnerId)
{
}

ParticleSystem::ParticleSystem(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId)
{
	myEmitters.reserve(1);
	AssetManager::GetInstance().LoadAsset<ParticleEmitter>(aFilePath,myEmitters[0]);
}

ParticleSystem::ParticleSystem(const unsigned int anOwnerId,const ParticleEmitterTemplate& aTemplate) : Component(anOwnerId)
{
	myEmitters.push_back(std::make_shared<ParticleEmitter>(aTemplate));
}
void ParticleSystem::Draw()
{
	for(auto& i : myEmitters)
	{
		i->Draw();
	}
}

void ParticleSystem::Update()
{
	for(auto& i : myEmitters)
	{
		i->Update(CU::Timer::GetInstance().GetDeltaTime());
	}
}