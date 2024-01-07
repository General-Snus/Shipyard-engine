#include "GraphicsEngine.pch.h"
#include "ParticleRenderer.h"
#include <Engine/AssetManager/ComponentSystem/Components/ParticleSystem.h>
void ParticleRenderer::Init()
{
	vertexShader = GraphicsEngine::Get().GetParticleVSShader();
	geometryShader = GraphicsEngine::Get().GetParticleGSShader();
	pixelShader = GraphicsEngine::Get().GetParticlePSShader();
}

void ParticleRenderer::Execute()
{
	RHI::SetVertexShader(vertexShader.Get());
	RHI::SetGeometryShader(geometryShader.Get());
	RHI::SetPixelShader(pixelShader.Get()); 

	for(auto& i : GameObjectManager::Get().GetAllComponents<ParticleSystem>())
	{
		i.Draw();
	}

	RHI::SetVertexShader(nullptr);
	RHI::SetGeometryShader(nullptr);
	RHI::SetPixelShader(nullptr);
}
