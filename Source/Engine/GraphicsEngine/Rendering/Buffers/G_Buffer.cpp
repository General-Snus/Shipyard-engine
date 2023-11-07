#include "GraphicsEngine.pch.h"
#include "G_Buffer.h"

#include <Shaders/Include/Default_VS.h> 
#include <Shaders/Include/GBufferPS.h>
#include <Shaders/Include/ScreenspaceQuad_VS.h>
#include <Shaders/Include/EnvironmentLight_PS.h>
#include <Shaders/Include/PointLight_PS.h> 
#include <Shaders/Include/SpotLight_PS.h> 
#include <Shaders/Include/DebugLayer_PS.h> 
#include <Engine/GraphicsEngine/GraphicsEngine.h> 
#include <d3d11.h>


void G_Buffer::Init()
{
	const int width = RHI::GetDeviceSize().Width;
	const int height = RHI::GetDeviceSize().Height;

	RHI::CreateVertexShader(
		myVertexShader,
		BuiltIn_Default_VS_ByteCode,
		sizeof(BuiltIn_Default_VS_ByteCode)
	);

	myScreenSpaceShader = GraphicsEngine::Get().GetQuadShader(); 

	RHI::CreatePixelShader(
		myPixelShader,
		BuiltIn_GBufferPS_ByteCode,
		sizeof(BuiltIn_GBufferPS_ByteCode)
	);

	RHI::CreatePixelShader(
		myEnviromentPixelShader,
		BuiltIn_EnvironmentLight_PS_ByteCode,
		sizeof(BuiltIn_EnvironmentLight_PS_ByteCode)
	);

	RHI::CreatePixelShader(
		mySpotShader,
		BuiltIn_SpotLight_PS_ByteCode,
		sizeof(BuiltIn_SpotLight_PS_ByteCode)
	);

	RHI::CreatePixelShader(
		myPointPixelShader,
		BuiltIn_PointLight_PS_ByteCode,
		sizeof(BuiltIn_PointLight_PS_ByteCode)
	); 

	RHI::CreatePixelShader(
		myDebugPixelShader,
		BuiltIn_DebugLayer_PS_ByteCode,
		sizeof(BuiltIn_DebugLayer_PS_ByteCode)
	);

	albedoTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		albedoTexture.get(),
		L"Default color texture",
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	normalTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		normalTexture.get(),
		L"Default normal texture",
		width,
		height,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	vertexNormalsTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		vertexNormalsTexture.get(),
		L"Default vertex normal texture",
		width,
		height,
		DXGI_FORMAT_R16G16B16A16_SNORM,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	materialTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		materialTexture.get(),
		L"Default material texture",
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	effectTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		effectTexture.get(),
		L"Default effect texture",
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	positionTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		positionTexture.get(),
		L"Default position texture",
		width,
		height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	depthTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		depthTexture.get(),
		L"Default depth texture",
		width,
		height,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	); 

	vectorOfTextures.push_back(albedoTexture.get());
	vectorOfTextures.push_back(normalTexture.get());
	vectorOfTextures.push_back(vertexNormalsTexture.get());
	vectorOfTextures.push_back(materialTexture.get());
	vectorOfTextures.push_back(effectTexture.get());
	vectorOfTextures.push_back(positionTexture.get());
	vectorOfTextures.push_back(depthTexture.get()); 

	//ID3D11Texture2D* depthStencilTexture = nullptr;
	//
	//D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	//depthStencilDesc.Width = RHI::GetDeviceSize().Width;
	//depthStencilDesc.Height= RHI::GetDeviceSize().Height;
	//depthStencilDesc.MipLevels = 1;
	//depthStencilDesc.ArraySize = 1;
	//depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//depthStencilDesc.SampleDesc.Count = 1; 
	//depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	//depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

}

void G_Buffer::SetWriteTargetToBuffer()
{ 
	for(int i = 0; i < vectorOfTextures.size(); i++)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,i,nullptr);
	}

	RHI::SetRenderTargets(vectorOfTextures,GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());
	
}

void G_Buffer::SetResourceTexture()
{
	for(int i = 0; i < vectorOfTextures.size(); i++)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,i,vectorOfTextures[i]);
	}
}

void G_Buffer::UnsetResources()
{
	for(int i = 0; i < vectorOfTextures.size(); i++)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,i,nullptr);
	}
}

void G_Buffer::UseGBufferShader()
{
	RHI::SetVertexShader(myVertexShader);
	RHI::SetPixelShader(myPixelShader);
}
void G_Buffer::UseEnviromentShader()
{ 
	RHI::SetVertexShader(myScreenSpaceShader);
	RHI::SetPixelShader(myEnviromentPixelShader);

	SetResourceTexture();
}


void G_Buffer::UseDebugShader()
{
	RHI::SetVertexShader(myScreenSpaceShader);
	RHI::SetPixelShader(myDebugPixelShader);

	SetResourceTexture();
}

void G_Buffer::UsePointlightShader()
{
	RHI::SetBlendState(GraphicsEngine::Get().GetAdditiveBlendState());
	RHI::SetVertexShader(myScreenSpaceShader); 
	RHI::SetPixelShader(myPointPixelShader);

	SetResourceTexture();
}

void G_Buffer::UseSpotlightShader()
{
	RHI::SetBlendState(GraphicsEngine::Get().GetAdditiveBlendState());
	RHI::SetVertexShader(myScreenSpaceShader);
	RHI::SetPixelShader(mySpotShader);

	SetResourceTexture();
}

void G_Buffer::ClearTargets()
{ 
	for(int i = 0; i < vectorOfTextures.size(); i++)
	{
		RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,i,nullptr);

		RHI::ClearRenderTarget(vectorOfTextures[i]);
	}

}
