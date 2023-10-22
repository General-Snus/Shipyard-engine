#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include <filesystem>
#include <utility>

#include <Shaders/Include/Default_PS.h>
#include <Shaders/Include/Default_VS.h>
#include <Shaders/Include/LuminancePass_PS.h>
#include <Shaders/Include/LinearToGammaPass.h>
#include <Shaders/Include/CopyPixels_PS.h> 
#include <Shaders/Include/GaussianBlur_PS.h> 
#include <Shaders/Include/Bloom_PS.h> 

#include <Shaders/Include/ParticleShader_VS.h> 
#include <Shaders/Include/ParticleShader_GS.h> 
#include <Shaders/Include/ParticleShader_PS.h> 

#include <Shaders/Include/Default_C.h>
#include <Shaders/Include/Default_N.h>
#include <Shaders/Include/Default_M.h>
#include <Shaders/Include/Default_FX.h>

#include <Shaders/Include/LineDrawer_PS.h>
#include <Shaders/Include/LineDrawer_VS.h>

#include <Shaders/Include/brdfLUT_PS.h>
#include <Shaders/Include/brdfLUT_VS.h>
#include <Shaders/Registers.h>

#include "Objects/Shader.h"

#include "Rendering/Vertex.h" 
#include "Rendering/ParticleRenderer/ParticleVertex.h" 
#include "GraphicCommands/GraphicCommands.h"
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/CameraComponent.h>

#include "GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_SetRenderTarget.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_DebugLayer.h"
#include "GraphicCommands/Commands/Headers/GfxCmd_SetLightBuffer.h" 
#include "GraphicCommands/Commands/Headers/GfxCmd_GaussianBlur.h" 

#include "Shaders/Registers.h"
#include <Tools/ImGui/imgui.h>
#include <stdexcept> 


bool GraphicsEngine::Initialize(HWND windowHandle, bool enableDeviceDebug)
{
	GELogger = Logger::Create("GraphicsEngine"); 
	DeferredCommandList.Initialize();
	OverlayCommandList.Initialize(); 

#ifdef _DEBUG
	try
	{
#endif
		GELogger.SetPrintToVSOutput(false);

		myWindowHandle = windowHandle;
		myBackBuffer = std::make_unique<Texture>();
		myDepthBuffer = std::make_unique<Texture>();

		if (!RHI::Initialize(myWindowHandle,
			enableDeviceDebug,
			myBackBuffer.get(),
			myDepthBuffer.get()))
		{
			GELogger.Err("Failed to initialize the RHI!");
			return false;
		}

		SetupDefaultVariables();
		SetupBRDF();
		SetupParticleShaders();
		SetupPostProcessing();
		SetupBlendStates(); 
		SetupDebugDrawline(); 

		myLightBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_LightBuffer, myLightBuffer);

		myObjectBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_ObjectBuffer, myObjectBuffer);

		myFrameBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_FrameBuffer, myFrameBuffer);

		myLineBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER, REG_LineBuffer, myLineBuffer);

		myG_Buffer.Init();
		myShadowRenderer.Init();
		myParticleRenderer.Init();
#ifdef _DEBUG
	}
	catch (const std::exception& e)
	{
		GELogger.LogException(e);
		exit(-1);
	}
#endif 
	return true;
}

bool GraphicsEngine::SetupDebugDrawline()
{ 
	debugLineVS = std::make_shared<Shader>();
	RHI::LoadShaderFromMemory(
		debugLineVS.get(),
		L"LineVertexShader",
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);

	debugLinePS = std::make_shared<Shader>();
	RHI::LoadShaderFromMemory(
		debugLinePS.get(),
		L"LinePixelShader",
		BuiltIn_LineDrawer_PS_ByteCode,
		sizeof(BuiltIn_LineDrawer_PS_ByteCode)
	);

	//Create DebugVertex input layout
	RHI::CreateInputLayout(
		Debug::DebugVertex::InputLayout,
		Debug::DebugVertex::InputLayoutDescription,
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);

	if (!(
		RHI::CreateDynamicVertexBuffer(myLineVertexBuffer, 65536, sizeof(Debug::DebugVertex)) &&
		RHI::CreateDynamicIndexBuffer(myLineIndexBuffer, 65536)
		))
	{
		GELogger.Err("Failed to initialize the myLineVertexBuffer!");
		return false;
	} 
	return true;
}

void GraphicsEngine::SetupDefaultVariables()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.f;
	samplerDesc.BorderColor[1] = 1.f;
	samplerDesc.BorderColor[2] = 1.f;
	samplerDesc.BorderColor[3] = 1.f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (!RHI::CreateSamplerState(myDefaultSampleState, samplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myDefaultSampleState, REG_DefaultSampler);

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 0.f;
	shadowSamplerDesc.BorderColor[1] = 0.f;
	shadowSamplerDesc.BorderColor[2] = 0.f;
	shadowSamplerDesc.BorderColor[3] = 0.f;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MipLODBias = 0.f;
	shadowSamplerDesc.MaxAnisotropy = 1;

	if (!RHI::CreateSamplerState(myShadowSampleState, shadowSamplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myShadowSampleState, REG_shadowCmpSampler);

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = false;

	auto result = RHI::Device->CreateDepthStencilState(
		&depthStencilDesc,
		&myDepthStencilStates[(int)eDepthStencilStates::DSS_ReadOnly]);
	if(FAILED(result))
	{
		GELogger.Log("Failed to create depth stencil read only state");
		assert(false);
	}
	myDepthStencilStates[(int)eDepthStencilStates::DSS_ReadWrite] = nullptr; 


	// TEMP: Load the default shader programs.
	// This will be done elsewhere later on :).
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

	defaultTexture = std::make_shared<TextureHolder>("", eTextureType::ColorMap);
	RHI::LoadTextureFromMemory(
		defaultTexture->GetRawTexture().get(),
		L"Default Color texture",
		BuiltIn_Default_C_ByteCode,
		sizeof(BuiltIn_Default_C_ByteCode)
	);

	defaultNormalTexture = std::make_shared<TextureHolder>("", eTextureType::NormalMap);
	RHI::LoadTextureFromMemory(
		defaultNormalTexture->GetRawTexture().get(),
		L"Default Normal texture",
		BuiltIn_Default_N_ByteCode,
		sizeof(BuiltIn_Default_N_ByteCode)
	);

	defaultMatTexture = std::make_shared<TextureHolder>("", eTextureType::MaterialMap);
	RHI::LoadTextureFromMemory(
		defaultMatTexture->GetRawTexture().get(),
		L"Default material texture",
		BuiltIn_Default_M_ByteCode,
		sizeof(BuiltIn_Default_M_ByteCode)
	);

	defaultEffectTexture = std::make_shared<TextureHolder>("", eTextureType::EffectMap);
	RHI::LoadTextureFromMemory(
		defaultEffectTexture->GetRawTexture().get(),
		L"Default effect texture",
		BuiltIn_Default_FX_ByteCode,
		sizeof(BuiltIn_Default_FX_ByteCode)
	);
	AssetManager::GetInstance().ForceLoadAsset<TextureHolder>(L"Textures/Default/DefaultParticle_P.dds",defaultParticleTexture);
	defaultParticleTexture->SetTextureType(eTextureType::ParticleMap); 

	defaultVS = std::make_shared<Shader>();
	defaultPS = std::make_shared<Shader>();

	defaultVS->SetShader(myVertexShader);
	defaultVS->myName = L"Default Vertex Shader";
	defaultPS->SetShader(myPixelShader);
	defaultPS->myName = L"Default Pixel Shader";

	AssetManager::GetInstance().ForceLoadAsset<Material>("Materials/Default.json", defaultMaterial);
	defaultMaterial->SetShader(defaultVS, defaultPS);

	AssetManager::GetInstance().ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds", defaultCubeMap);
	defaultCubeMap->SetTextureType(eTextureType::CubeMap);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, REG_enviromentCube, defaultCubeMap->GetRawTexture().get());

	AssetManager::GetInstance().ForceLoadAsset<Mesh>("default.fbx", defaultMesh);
}

void GraphicsEngine::SetupBlendStates()
{
	D3D11_BLEND_DESC blendDesc = {};
	D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
	rtBlendDesc.BlendEnable = TRUE;
	rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if(!RHI::CreateBlendState(AlphaBlendState,blendDesc))
	{
		assert(false);
	}

	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if(!RHI::CreateBlendState(AdditiveBlendState,blendDesc))
	{
		assert(false);
	}
}
void GraphicsEngine::SetupBRDF()
{
	//Light
	BRDLookUpTable = std::make_shared<Texture>();
	RHI::CreateTexture(BRDLookUpTable.get(), L"brdfLUT", 512, 512,
		DXGI_FORMAT_R16G16_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
	);
	RHI::ClearRenderTarget(BRDLookUpTable.get());

	ComPtr<ID3D11PixelShader> brdfPS;

	RHI::CreateVertexShader(
		myScreenSpaceQuadShader,
		BuiltIn_brdfLUT_VS_ByteCode,
		sizeof(BuiltIn_brdfLUT_VS_ByteCode)
	);

	RHI::CreatePixelShader(
		brdfPS,
		BuiltIn_brdfLUT_PS_ByteCode,
		sizeof(BuiltIn_brdfLUT_PS_ByteCode)
	);
	RHI::SetVertexShader(myScreenSpaceQuadShader);
	RHI::SetPixelShader(brdfPS);

	RHI::SetRenderTarget(BRDLookUpTable.get(), nullptr);

	RHI::ConfigureInputAssembler(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr, nullptr, 0, nullptr);
	RHI::Draw(4);

	RHI::SetRenderTarget(nullptr, nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, REG_BRDF_LUT_Texture, BRDLookUpTable.get());


	D3D11_SAMPLER_DESC lutsamplerDesc = {};
	lutsamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutsamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.MipLODBias = 0.f;
	lutsamplerDesc.MaxAnisotropy = 1;
	lutsamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutsamplerDesc.BorderColor[0] = 1.f;
	lutsamplerDesc.BorderColor[1] = 1.f;
	lutsamplerDesc.BorderColor[2] = 1.f;
	lutsamplerDesc.BorderColor[3] = 1.f;
	lutsamplerDesc.MinLOD = 0;
	lutsamplerDesc.MaxLOD = 0;

	if (!RHI::CreateSamplerState(myBRDFSampleState, lutsamplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	}

	RHI::SetSamplerState(myBRDFSampleState, REG_BRDFSampler);

}
  
void GraphicsEngine::SetupPostProcessing()
{
	RHI::DeviceSize size = RHI::GetDeviceSize();
	SceneBuffer = std::make_shared<Texture>();
	RHI::CreateTexture(
		SceneBuffer.get(),
		L"SceneBuffer",
		size.Width, size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	halfSceneBuffer = std::make_shared<Texture>();
	RHI::CreateTexture(
		halfSceneBuffer.get(),
		L"SceneBuffer",
		size.Width / 2, size.Height / 2,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	quaterSceneBuffer1 = std::make_shared<Texture>();
	RHI::CreateTexture(
		quaterSceneBuffer1.get(),
		L"SceneBuffer",
		size.Width / 4, size.Height / 4,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	quaterSceneBuffer2 = std::make_shared<Texture>();
	RHI::CreateTexture(
		quaterSceneBuffer2.get(),
		L"SceneBuffer",
		size.Width / 4, size.Height / 4,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	IntermediateA = std::make_shared<Texture>();
	RHI::CreateTexture(
		IntermediateA.get(),
		L"IntermediateA",
		size.Width, size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	IntermediateB = std::make_shared<Texture>();
	RHI::CreateTexture(
		IntermediateB.get(),
		L"IntermediateB",
		size.Width, size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	RHI::CreatePixelShader(
		luminancePass,
		BuiltIn_LuminancePass_PS_ByteCode,
		sizeof(BuiltIn_LuminancePass_PS_ByteCode)
	);
	RHI::CreatePixelShader(
		linearGammaPass,
		BuiltIn_LinearToGammaPass_ByteCode,
		sizeof(BuiltIn_LinearToGammaPass_ByteCode)
	);

	RHI::CreatePixelShader(
		copyShader,
		BuiltIn_CopyPixels_PS_ByteCode,
		sizeof(BuiltIn_CopyPixels_PS_ByteCode)
	);

	RHI::CreatePixelShader(
		gaussShader,
		BuiltIn_GaussianBlur_PS_ByteCode,
		sizeof(BuiltIn_GaussianBlur_PS_ByteCode)
	);

	RHI::CreatePixelShader(
		bloomShader,
		BuiltIn_Bloom_PS_ByteCode,
		sizeof(BuiltIn_Bloom_PS_ByteCode)
	);

}

void GraphicsEngine::SetupParticleShaders()
{
	RHI::CreateVertexShaderAndInputLayout(
		particleVertexShader,
		Particlevertex::InputLayout,
		Particlevertex::InputLayoutDefinition,
		BuiltIn_ParticleShader_VS_ByteCode,
		sizeof(BuiltIn_ParticleShader_VS_ByteCode)
	);
	RHI::CreateGeometryShader(
		particleGeometryShader,
		BuiltIn_ParticleShader_GS_ByteCode,
		sizeof(BuiltIn_ParticleShader_GS_ByteCode)
	);
	RHI::CreatePixelShader(
		particlePixelShader,
		BuiltIn_ParticleShader_PS_ByteCode,
		sizeof(BuiltIn_ParticleShader_PS_ByteCode)
	); 
}

void GraphicsEngine::SetLoggingWindow(HANDLE aHandle)  const
{
	GELogger.SetConsoleHandle(aHandle);
}

void GraphicsEngine::BeginFrame()
{
	myCamera = GameObjectManager::GetInstance().GetCamera().TryGetComponent<cCamera>();
	if (!myCamera)
	{
		GELogger.Err("No camera in scene. No render is possible");
	}

	// Here we should initialize our frame and clean up from the last one.  
	RHI::ClearRenderTarget(myBackBuffer.get(), myBackgroundColor);
	RHI::ClearDepthStencil(myDepthBuffer.get());

	RHI::ClearRenderTarget(SceneBuffer.get(), { 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(halfSceneBuffer.get(), { 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer1.get(), { 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer2.get(), { 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateA.get(), { 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateB.get(), { 0.0f,0.0f,0.0f,0.0f });

	myG_Buffer.ClearTargets();
	RHI::SetBlendState(nullptr);
}

void GraphicsEngine::RenderFrame(float aDeltaTime, double aTotalTime)
{
	aDeltaTime; aTotalTime;
	RHI::SetVertexShader(myVertexShader);

	myCamera->SetCameraToFrameBuffer();
	myG_Buffer.SetWriteTargetToBuffer(); //Let all write to textures
	DeferredCommandList.Execute();

	//decals
	//if picking check
	//SSAO
	//Do ambience pass? Clarit

	//Render shadowmaps
	myShadowRenderer.Execute();

	//Render all lights
	myCamera->SetCameraToFrameBuffer();
	GfxCmd_SetRenderTarget(SceneBuffer.get(), nullptr).ExecuteAndDestroy();
	GfxCmd_SetLightBuffer().ExecuteAndDestroy(); //REFACTOR Change name to fit purpose

	//Forward pass for light

	//Particles
	RHI::SetBlendState(GraphicsEngine::Get().GetAdditiveBlendState());
	GfxCmd_SetRenderTarget(SceneBuffer.get(),myDepthBuffer.get()).ExecuteAndDestroy();
	myParticleRenderer.Execute();

	//Post processing
	RHI::SetBlendState(nullptr);
	GfxCmd_LuminancePass().ExecuteAndDestroy(); // Render to IntermediateA
	GfxCmd_GaussianBlur().ExecuteAndDestroy();
	GfxCmd_Bloom().ExecuteAndDestroy();
	GfxCmd_LinearToGamma().ExecuteAndDestroy(); // Render: BakcBuffer Read: REG_Target01

	//Debug layers 
	GfxCmd_DebugLayer().ExecuteAndDestroy();
	OverlayCommandList.Execute();
}

void GraphicsEngine::RenderTextureTo(eRenderTargets from, eRenderTargets to)  const
{
	const Texture* texture1 = GraphicsEngine::Get().GetTargetTextures(from).get();
	const Texture* texture2 = GraphicsEngine::Get().GetTargetTextures(to).get();

	RHI::SetRenderTarget(texture2, nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, REG_Target0, texture1);
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER, REG_Target0, nullptr);
}

void GraphicsEngine::EndFrame()
{
	// We finish our frame here and present it on screen.
	RHI::Present(0);
	myShadowRenderer.ResetShadowList();
	DeferredCommandList.Reset();
	OverlayCommandList.Reset();
}

