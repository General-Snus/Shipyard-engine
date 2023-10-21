#include "AssetManager.pch.h"
#include "ParticleSystem.h"
#include <Tools/Utilities/Game/Timer.h>

ParticleSystem::ParticleSystem(const unsigned int anOwnerId) : Component(anOwnerId)
{
}

ParticleSystem::ParticleSystem(const unsigned int anOwnerId,const std::filesystem::path& aFilePath) : Component(anOwnerId)
{
	aFilePath;
	//myEmitters.reserve(1);
	//AssetManager::GetInstance().LoadAsset<ParticleEmitter>(aFilePath,myEmitters[0]);
}
 
void ParticleSystem::Draw()
{
	for(auto& i : myEmitters)
	{
		i->Draw();
	}
}

void ParticleSystem::AddEmitter(const ParticleEmitterTemplate& aTemplate)
{
	myEmitters.push_back(std::make_shared<ParticleEmitter>(aTemplate));
}
void ParticleSystem::Update()
{
	for(auto& i : myEmitters)
	{
		i->Update(CU::Timer::GetInstance().GetDeltaTime());
	}
}