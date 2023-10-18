#include <AssetManager.pch.h>
#include "ParticleEmitter.h"

void ParticleEmitter::InitParticle(size_t aParticleIndex)
{
	aParticleIndex;
}

ParticleEmitter::~ParticleEmitter()
{
}

ParticleEmitter::ParticleEmitter(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	throw std::exception("Not implemented");
}

ParticleEmitter::ParticleEmitter(const ParticleEmitterTemplate& aTemplate) : AssetBase(aTemplate.Path)
{
	data = aTemplate.EmmiterSettings;

	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	if(data.MaxParticles == 0)
	{
		data.MaxParticles = (int)std::ceil(data.SpawnRate * data.LifeTime);
	}

	particles.resize(data.MaxParticles);

	for(auto& part : particles)
	{
		part.Color = data.StartColor;
		part.Lifetime = data.LifeTime;
		part.Position = {0,0,0,1};
		part.Scale = data.StartSize;
		part.Velocity = data.StartVelocity;
	}
	 
	
	vertexShader = GraphicsEngine::Get().GetParticleVSShader();
	geometryShader = GraphicsEngine::Get().GetParticleGSShader();
	pixelShader = GraphicsEngine::Get().GetParticlePSShader();
	stride = sizeof(Particlevertex); 
}

void ParticleEmitter::Init()
{
	return ;
}

void ParticleEmitter::Draw()
{
}

void ParticleEmitter::SetAsResource() const
{
}
