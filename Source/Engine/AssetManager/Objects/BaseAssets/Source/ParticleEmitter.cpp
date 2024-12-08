#include "AssetManager.pch.h"

#include "../ParticleEmitter.h"
#include <Tools/ImGui/imgui.h>
#include <Tools/Utilities/Math.hpp>
#include "Engine/AssetManager/Objects/BaseAssets/TextureAsset.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"

#include "Tools/Utilities/LinearAlgebra/Easing.h"


void ParticleEmitter::InitParticle(Particlevertex& vertex) const
{
	vertex.Color = settings.StartColor;
	vertex.Lifetime = 0;
	vertex.Position = settings.StartPosition;
	vertex.Scale = settings.StartSize;
	vertex.Velocity = settings.StartVelocity;
}

ParticleEmitter::ParticleEmitter(const std::filesystem::path& aFilePath) : AssetBase(aFilePath), stride(0), offset(0),
                                                                           primitiveTopology(0)
{
	LOGGER.Err("NotImplementedException");
	assert(false);
}

ParticleEmitter::ParticleEmitter(const ParticleEmitterTemplate& aTemplate) : AssetBase(aTemplate.Path)
{
	if (aTemplate.EmmiterSettings.ParticleTexture.empty())
	{
		texture = GraphicsEngineInstance.GetDefaultTexture(eTextureType::ParticleMap);
	}
	else
	{
		texture = ENGINE_RESOURCES.LoadAsset<TextureHolder>(aTemplate.EmmiterSettings.ParticleTexture);
	}

	settings = aTemplate.EmmiterSettings;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	if (settings.MaxParticles == 0 || static_cast<int>(std::ceil(settings.SpawnRate * settings.LifeTime)) < settings.
		MaxParticles)
	{
		settings.MaxParticles = static_cast<int>(std::ceil(settings.SpawnRate * settings.LifeTime));
	}

	particles.resize(settings.MaxParticles);

	for (auto& part : particles)
	{
		InitParticle(part);
		part.Lifetime = settings.LifeTime + 1;
	}

	//if (!RHI::CreateDynamicVertexBuffer(vertexBuffer,particles.size(),sizeof(Particlevertex)))
	{
		LOGGER.Err("Failed to create vertex buffer");
		return;
	}
	//stride = sizeof(Particlevertex);
}

void ParticleEmitter::Init()
{
}

void ParticleEmitter::Update(float aDeltaTime)
{
	aDeltaTime = aDeltaTime * settings.SimulationSpeed;

	secondCounter += settings.SpawnRate * aDeltaTime;
	spawnedThisFrame = std::floor(secondCounter) + 0.1f; //point exactness
	secondCounter -= spawnedThisFrame;

	for (auto& aParticle : particles)
	{
		aParticle.Lifetime += aDeltaTime;
		if (aParticle.Lifetime >= this->settings.LifeTime && spawnedThisFrame > 1)
		{
			spawnedThisFrame--;
			InitParticle(aParticle);
		}

		aParticle.Velocity += settings.Acceleration * aDeltaTime;

		aParticle.Position.x += aParticle.Velocity.x * aDeltaTime;
		aParticle.Position.y += aParticle.Velocity.y * aDeltaTime;
		aParticle.Position.z += aParticle.Velocity.z * aDeltaTime;

		aParticle.Color = lerp(settings.StartColor, settings.EndColor, aParticle.Lifetime / settings.LifeTime);
		float scaleAllAxis = lerp(settings.StartSize, settings.EndSize, aParticle.Lifetime / settings.LifeTime);
		aParticle.Scale = {scaleAllAxis, scaleAllAxis, scaleAllAxis};
	}
}

void ParticleEmitter::Draw()
{
	SetAsResource();
	//RHI::Draw(static_cast<unsigned int>(particles.size()));
}

void ParticleEmitter::SetAsResource() const
{
	//auto result = ((HRESULT)1L);

	//D3D11_MAPPED_SUBRESOURCE bufferData;
	//ZeroMemory(&bufferData,sizeof(D3D11_MAPPED_SUBRESOURCE));

	//result = RHI::Context->Map(
	//	vertexBuffer.Get(),
	//	0,
	//	D3D11_MAP_WRITE_DISCARD,
	//	0,
	//	&bufferData);

	//if (FAILED(result))
	//{
	//	Logger.Log("Failed to create vertex buffer for particle emitter");
	//	assert(false);
	//}

	//memcpy_s(
	//	bufferData.pData,
	//	sizeof(Particlevertex) * particles.size(),
	//	&particles[0],
	//	sizeof(Particlevertex) * particles.size()
	//);

	//RHI::Context->Unmap(vertexBuffer.Get(),0);

	///*RHI::ConfigureInputAssembler
	//(
	//	primitiveTopology,
	//	vertexBuffer,
	//	nullptr,
	//	stride,
	//	Particlevertex::InputLayout
	//);*/

	//if (texture->isLoadedComplete)
	//{
	//	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_colorMap,texture->GetRawTexture().get());
	//}
}
