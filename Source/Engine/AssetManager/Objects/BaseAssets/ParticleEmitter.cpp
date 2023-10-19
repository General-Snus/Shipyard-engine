#include <AssetManager.pch.h>
#include "ParticleEmitter.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <Tools/Utilities/Math.hpp>
#include <d3d11.h>



void ParticleEmitter::InitParticle(Particlevertex vertex)
{
	vertex.Color = settings.StartColor;
	vertex.Lifetime = settings.LifeTime;
	vertex.Position = {0,0,0,1};
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
	settings = aTemplate.EmmiterSettings;

	primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	if(settings.MaxParticles == 0)
	{
		settings.MaxParticles = (int)std::ceil(settings.SpawnRate * settings.LifeTime);
	}

	particles.resize(settings.MaxParticles);

	for(auto& part : particles)
	{
		part.Color = settings.StartColor;
		part.Lifetime = settings.LifeTime;
		part.Position = {0,0,0,1};
		part.Scale = settings.StartSize;
		part.Velocity = settings.StartVelocity;
	}



	stride = sizeof(Particlevertex);
}

void ParticleEmitter::Init()
{
	return;
}

void ParticleEmitter::Update(float aDeltaTime)
{
	for(auto& aParticle : particles)
	{
		aParticle.Lifetime += aDeltaTime;

		if(aParticle.Lifetime >= this->settings.LifeTime)
		{
			InitParticle(aParticle);
		}

		aParticle.Velocity += settings.Acceleration * aDeltaTime;

		aParticle.Position.x += aParticle.Velocity.x * aDeltaTime;
		aParticle.Position.y += aParticle.Velocity.y * aDeltaTime;
		aParticle.Position.z += aParticle.Velocity.z * aDeltaTime;

		//aParticle.Color = CU::Lerp(settings.StartColor,settings.EndColor,aParticle.Lifetime / settings.LifeTime);
		//aParticle.Scale = Lerp(settings.StartSize,settings.EndSize,aParticle.Lifetime / settings.LifeTime);

	}
}

void ParticleEmitter::Draw()
{
	SetAsResource();
	RHI::Draw(0);
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

	RHI::Context->IASetInputLayout(inputLayout.Get());
	RHI::Context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)primitiveTopology);
	RHI::Context->IASetVertexBuffers(0,1,vertexBuffer.GetAddressOf(),&stride,&offset);

	if(texture)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER,REG_colorMap,texture.get());
	}
}
