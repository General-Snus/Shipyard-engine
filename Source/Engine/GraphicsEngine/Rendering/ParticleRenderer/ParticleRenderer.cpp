#include "GraphicsEngine.pch.h"
#include "ParticleRenderer.h"
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/ParticleSystem.h>
void ParticleRenderer::Init()
{
	vertexShader = GraphicsEngine::Get().GetParticleVSShader();
	geometryShader = GraphicsEngine::Get().GetParticleGSShader();
	pixelShader = GraphicsEngine::Get().GetParticlePSShader();
}

void ParticleRenderer::Execute()
{
	std::shared_ptr<Texture> shadowMap;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<ParticleSystem>())
	{
		i.Draw();
	}
}
