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
	RHI::LoadTextureFromMemory(
		albedoTexture.get(),
		L"Default Color texture",
		BuiltIn_Default_C_ByteCode,
		sizeof(BuiltIn_Default_C_ByteCode)
	);

	normalTexture = std::make_shared<Texture>();
	RHI::LoadTextureFromMemory(
		normalTexture.get(),
		L"Default Normal texture",
		BuiltIn_Default_N_ByteCode,
		sizeof(BuiltIn_Default_N_ByteCode)
	);

	materialTexture = std::make_shared<Texture>();
	RHI::LoadTextureFromMemory(
		materialTexture.get(),
		L"Default material texture",
		BuiltIn_Default_M_ByteCode,
		sizeof(BuiltIn_Default_M_ByteCode)
	);

	effectTexture = std::make_shared<Texture>();
	RHI::LoadTextureFromMemory(
		effectTexture.get(),
		L"Default effect texture",
		BuiltIn_Default_FX_ByteCode,
		sizeof(BuiltIn_Default_FX_ByteCode)
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
