#include "GraphicsEngine.pch.h"
#include "G_Buffer.h"

#include <Shaders/Include/Default_PS.h>
#include <Shaders/Include/Default_VS.h>

#include <Shaders/Include/Default_C.h>
#include <Shaders/Include/Default_N.h>
#include <Shaders/Include/Default_M.h>
#include <Shaders/Include/Default_FX.h>
#include <d3d11.h>

void G_BufferData::Init()
{
	const int width = RHI::GetDeviceSize().Width;
	const int height = RHI::GetDeviceSize().Height;

	RHI::CreateVertexShaderAndInputLayout(
		myVertexShader,
		Vertex::InputLayout,
		Vertex::InputLayoutDefinition,
		BuiltIn_Default_VS_ByteCode,
		sizeof(BuiltIn_Default_VS_ByteCode)
	);
	RHI::CreatePixelShader(
		myPixelShader,
		BuiltIn_Default_PS_ByteCode,
		sizeof(BuiltIn_Default_PS_ByteCode)
	);
	RHI::CreateInputLayout(
		Vertex::InputLayout,
		Vertex::InputLayoutDefinition,
		BuiltIn_Default_VS_ByteCode,
		sizeof(BuiltIn_Default_VS_ByteCode)
	);

	albedoTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		albedoTexture.get(),
		L"Default color texture",
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D11_USAGE_DEFAULT,
		0,
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
		0,
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
		0,
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
		0,
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
		0,
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
		0,
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
		0,
		0
	);

	ID3D11Texture2D* depthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC depthStencilDesc = {};
	depthStencilDesc.Width = RHI::GetDeviceSize().Width;
	depthStencilDesc.Height= RHI::GetDeviceSize().Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1; 
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

}

void G_BufferData::SetMaterialBuffer(std::vector<std::shared_ptr<TextureHolder>> textures)
{
}
