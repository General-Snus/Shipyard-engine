#include <AssetManager.pch.h>
#include "ParticleEmitter.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <Tools/Utilities/Math.hpp>
#include <d3d11.h>
#include <random>

#include <Tools/ImGui/imgui.h>



void ParticleEmitter::InitParticle(Particlevertex& vertex)
{
	vertex.Color = settings.StartColor;
	vertex.Lifetime = 0;
	vertex.Position = settings.StartPosition;
	vertex.Scale = settings.StartSize;



	vertex.Velocity = settings.StartVelocity;
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
	if(aTemplate.EmmiterSettings.ParticleTexture.empty())
	{
		texture = GraphicsEngine::Get().GetDefaultTexture(eTextureType::ParticleMap);
	}
	else
	{
		AssetManager::GetInstance().LoadAsset<TextureHolder>(aTemplate.EmmiterSettings.ParticleTexture,texture);
	}

	settings = aTemplate.EmmiterSettings;
	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	if(settings.MaxParticles == 0 || (int)std::ceil(settings.SpawnRate * settings.LifeTime) < settings.MaxParticles)
	{
		settings.MaxParticles = (int)std::ceil(settings.SpawnRate * settings.LifeTime);
	}

	particles.resize(settings.MaxParticles);

	for(auto& part : particles)
	{
		InitParticle(part);
		part.Lifetime = settings.LifeTime + 1;
	}

	if(!RHI::CreateDynamicVertexBuffer(vertexBuffer,particles.size(),sizeof(Particlevertex)))
	{
		std::cout << "Failed to create vertex buffer" << std::endl;
		return;
	}
	inputLayout = Particlevertex::InputLayout;
	stride = sizeof(Particlevertex);
}

void ParticleEmitter::Init()
{
	return;
}

void ParticleEmitter::Update(float aDeltaTime)
{  
	aDeltaTime = aDeltaTime * settings.SimulationSpeed;

	secondCounter += settings.SpawnRate * aDeltaTime;
	spawnedThisFrame = std::floor(secondCounter)+0.1f;//point exactness
	secondCounter -= spawnedThisFrame;

	for(auto& aParticle : particles)
	{
		aParticle.Lifetime += aDeltaTime;
		if(aParticle.Lifetime >= this->settings.LifeTime && spawnedThisFrame > 1)
		{
			spawnedThisFrame--;
			InitParticle(aParticle);
		}

		aParticle.Velocity += settings.Acceleration * aDeltaTime;

		aParticle.Position.x += aParticle.Velocity.x * aDeltaTime;
		aParticle.Position.y += aParticle.Velocity.y * aDeltaTime;
		aParticle.Position.z += aParticle.Velocity.z * aDeltaTime;

		aParticle.Color = CU::Lerp(settings.StartColor,settings.EndColor,aParticle.Lifetime / settings.LifeTime);
		float scaleAllAxis = Lerp(settings.StartSize,settings.EndSize,aParticle.Lifetime / settings.LifeTime);
		aParticle.Scale = {scaleAllAxis,scaleAllAxis,scaleAllAxis};
	}
}

void ParticleEmitter::Draw()
{
	SetAsResource();
	RHI::Draw(static_cast<unsigned int>(particles.size()));
}

void ParticleEmitter::SetAsResource() const
{
	HRESULT result = S_FALSE;

	D3D11_MAPPED_SUBRESOURCE bufferData;
	ZeroMemory(&bufferData,sizeof(D3D11_MAPPED_SUBRESOURCE));

	result = RHI::Context->Map(
		vertexBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&bufferData);

	memcpy_s(
		bufferData.pData,
		sizeof(Particlevertex) * particles.size(),
		&particles[0],
		sizeof(Particlevertex) * particles.size()
	);

	RHI::Context->Unmap(vertexBuffer.Get(),0);

	RHI::ConfigureInputAssembler
	(
		primitiveTopology,
		vertexBuffer,
		nullptr,
		stride,
		Particlevertex::InputLayout
	);

	if(texture->isLoadedComplete)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_colorMap,texture->GetRawTexture().get()); 
	}
}
