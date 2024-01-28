#include "GraphicsEngine.pch.h"
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <Engine/GraphicsEngine/InterOp/XTK/DDSTextureLoader.h>
#include <Objects/DataObjects/Default_C.h>
#include <Objects/DataObjects/Default_FX.h>
#include <Objects/DataObjects/Default_M.h>
#include <Objects/DataObjects/Default_N.h>
#include <Shaders/Include/Bloom_PS.h> 
#include <Shaders/Include/brdfLUT_PS.h>
#include <Shaders/Include/CopyPixels_PS.h> 
#include <Shaders/Include/Default_PS.h>
#include <Shaders/Include/Default_VS.h>
#include <Shaders/Include/EdgeBlur.h> 
#include <Shaders/Include/GaussianBlur_PS.h> 
#include <Shaders/Include/LineDrawer_PS.h>
#include <Shaders/Include/LineDrawer_VS.h>
#include <Shaders/Include/LuminancePass_PS.h>
#include <Shaders/Include/ParticleShader_GS.h> 
#include <Shaders/Include/ParticleShader_PS.h> 
#include <Shaders/Include/ParticleShader_VS.h> 
#include <Shaders/Include/ScreenspaceQuad_VS.h>
#include <Shaders/Include/SSAO_PS.h> 
#include <Shaders/Include/ToneMapping_PS.h>
#include <stdexcept> 
#include <Tools/ImGui/ImGui/imgui.h>
#include <Tools/Optick/include/optick.h> 
#include "GraphicsEngine.h"     

#include "InterOp/GPU.h"
#include "InterOp/PSO.h"


bool GraphicsEngine::Initialize(HWND windowHandle,bool enableDeviceDebug)
{
	DeferredCommandList.Initialize();
	OverlayCommandList.Initialize();

#ifdef _DEBUG
	try
	{
#endif 
		myWindowHandle = windowHandle;
		myBackBuffer = std::make_unique<Texture>();
		myDepthBuffer = std::make_unique<Texture>();

		if (!GPU::Initialize(myWindowHandle,
			enableDeviceDebug,
			myBackBuffer.get(),
			myDepthBuffer.get()))
		{
			Logger::Err("Failed to initialize the DX12 GPU!");
			return false;
		}

		if (!RHI::Initialize(myWindowHandle,
			enableDeviceDebug,
			myBackBuffer.get(),
			myDepthBuffer.get()))
		{
			Logger::Err("Failed to initialize the RHI!");
			return false;
		}
		m_StateCache = std::make_unique<PSOCache>();
		m_StateCache->InitAllStates();

		SetupDefaultVariables();
		SetupBRDF();
		SetupParticleShaders();
		SetupPostProcessing();
		SetupBlendStates();
		SetupDebugDrawline();

		myLightBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_LightBuffer,myLightBuffer);

		myObjectBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_ObjectBuffer,myObjectBuffer);

		myFrameBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_FrameBuffer,myFrameBuffer);

		myLineBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_LineBuffer,myLineBuffer);

		myGraphicSettingsBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER,REG_GraphicSettingsBuffer,myGraphicSettingsBuffer);

		myG_Buffer.Init();
		myShadowRenderer.Init();
		myParticleRenderer.Init();
		RHI::SetDepthState(DepthState::DS_Reversed);

#ifdef _DEBUG
	}
	catch (const std::exception& e)
	{
		Logger::LogException(e);
		exit(-1);
	}
#endif 
	return true;
}

bool GraphicsEngine::SetupDebugDrawline()
{
	DebugDrawer::Get().Initialize();
	DebugDrawer::Get().AddDebugGrid({ 0.f, 0.0f, 0.f },1000,10,{ 1.0f, 1.0f, 1.0f });
	return true;
}

void GraphicsEngine::SetupDefaultVariables()
{
	D3D12_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.BorderColor[0] = 1.f;
	samplerDesc.BorderColor[1] = 1.f;
	samplerDesc.BorderColor[2] = 1.f;
	samplerDesc.BorderColor[3] = 1.f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (!RHI::CreateSamplerState(myDefaultSampleState,samplerDesc))
	{
		Logger::Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myDefaultSampleState,REG_DefaultSampler);

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 1.f;
	shadowSamplerDesc.BorderColor[1] = 1.f;
	shadowSamplerDesc.BorderColor[2] = 1.f;
	shadowSamplerDesc.BorderColor[3] = 1.f;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;/*
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MipLODBias = 0.f;
	shadowSamplerDesc.MaxAnisotropy = 1;*/

	if (!RHI::CreateSamplerState(myShadowSampleState,shadowSamplerDesc))
	{
		Logger::Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myShadowSampleState,REG_shadowCmpSampler);

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;



	//auto result = RHI::Device->CreateDepthStencilState(
	//	&depthStencilDesc,
	//	&myDepthStencilStates[(int)eDepthStencilStates::DSS_ReadOnly]
	//);
	if (GPU::CreateDepthStencil(depthStencilDesc))
	{
		Logger::Log("Failed to create depth stencil read only state");
		assert(false);
	}
	myDepthStencilStates[(int)eDepthStencilStates::DSS_ReadWrite] = nullptr;

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

	defaultTexture = std::make_shared<TextureHolder>("",eTextureType::ColorMap);

	if (!RHI::LoadTexture(
		defaultTexture->GetRawTexture().get(),
		AssetManager::Get().AssetPath / "Textures/Default/DefaultTile.dds"))
	{
		RHI::LoadTextureFromMemory(
			defaultTexture->GetRawTexture().get(),
			L"Default Color texture",
			BuiltIn_Default_C_ByteCode,
			sizeof(BuiltIn_Default_C_ByteCode)
		);
	}


	defaultNormalTexture = std::make_shared<TextureHolder>("",eTextureType::NormalMap);
	RHI::LoadTextureFromMemory(
		defaultNormalTexture->GetRawTexture().get(),
		L"Default Normal texture",
		BuiltIn_Default_N_ByteCode,
		sizeof(BuiltIn_Default_N_ByteCode)
	);

	defaultMatTexture = std::make_shared<TextureHolder>("",eTextureType::MaterialMap);
	RHI::LoadTextureFromMemory(
		defaultMatTexture->GetRawTexture().get(),
		L"Default material texture",
		BuiltIn_Default_M_ByteCode,
		sizeof(BuiltIn_Default_M_ByteCode)
	);

	defaultEffectTexture = std::make_shared<TextureHolder>("",eTextureType::EffectMap);
	RHI::LoadTextureFromMemory(
		defaultEffectTexture->GetRawTexture().get(),
		L"Default effect texture",
		BuiltIn_Default_FX_ByteCode,
		sizeof(BuiltIn_Default_FX_ByteCode)
	);

	//Particle
	AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/DefaultParticle_P.dds",defaultParticleTexture);
	defaultParticleTexture->SetTextureType(eTextureType::ParticleMap);




	//NOISE
	D3D11_SAMPLER_DESC pointSamplerDesc = {};
	pointSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	pointSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	if (!RHI::CreateSamplerState(myPointSampleState,pointSamplerDesc))
	{
		Logger::Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myPointSampleState,REG_PointSampler);


	AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/NoiseTable.dds",NoiseTable);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Noise_Texture,NoiseTable->GetRawTexture().get()); //Is there guarantee that this holds?

	defaultVS = std::make_shared<Shader>();
	defaultPS = std::make_shared<Shader>();

	defaultVS->SetShader(myVertexShader);
	defaultVS->myName = L"Default Vertex Shader";
	defaultPS->SetShader(myPixelShader);
	defaultPS->myName = L"Default Pixel Shader";

	AssetManager::Get().ForceLoadAsset<Material>("Materials/Default.json",defaultMaterial);
	defaultMaterial->SetShader(defaultVS,defaultPS);

	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds",defaultCubeMap);
	defaultCubeMap->SetTextureType(eTextureType::CubeMap);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_enviromentCube,defaultCubeMap->GetRawTexture().get());

	AssetManager::Get().ForceLoadAsset<Mesh>("default.fbx",defaultMesh);
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

	if (!RHI::CreateBlendState(AlphaBlendState,blendDesc))
	{
		assert(false);
	}

	rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
	rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (!RHI::CreateBlendState(AdditiveBlendState,blendDesc))
	{
		assert(false);
	}
}

void GraphicsEngine::SetupBRDF()
{
	//Light
	BRDLookUpTable = std::make_shared<Texture>();
	RHI::CreateTexture(BRDLookUpTable.get(),L"brdfLUT",512,512,
		DXGI_FORMAT_R16G16_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
	);
	RHI::ClearRenderTarget(BRDLookUpTable.get());

	ComPtr<ID3D11PixelShader> brdfPS;

	RHI::CreateVertexShader(
		myScreenSpaceQuadShader,
		BuiltIn_ScreenspaceQuad_VS_ByteCode,
		sizeof(BuiltIn_ScreenspaceQuad_VS_ByteCode)
	);

	RHI::CreatePixelShader(
		brdfPS,
		BuiltIn_brdfLUT_PS_ByteCode,
		sizeof(BuiltIn_brdfLUT_PS_ByteCode)
	);
	RHI::SetVertexShader(myScreenSpaceQuadShader);
	RHI::SetPixelShader(brdfPS);

	RHI::SetRenderTarget(BRDLookUpTable.get(),nullptr);

	RHI::ConfigureInputAssembler(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,nullptr,0,nullptr);
	RHI::Draw(4);

	RHI::SetRenderTarget(nullptr,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_BRDF_LUT_Texture,BRDLookUpTable.get());


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

	if (!RHI::CreateSamplerState(myBRDFSampleState,lutsamplerDesc))
	{
		Logger::Log("Sampler state created");
		assert(false);
	}

	RHI::SetSamplerState(myBRDFSampleState,REG_BRDFSampler);

}

void GraphicsEngine::SetupPostProcessing()
{
	D3D11_SAMPLER_DESC normalDepthSampler = {};
	normalDepthSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	normalDepthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	normalDepthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	normalDepthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	if (!RHI::CreateSamplerState(myNormalDepthSampleState,normalDepthSampler))
	{
		Logger::Log("Sampler state created");
		assert(false);
	}

	RHI::SetSamplerState(myNormalDepthSampleState,REG_normalDepthSampler);

	RHI::DeviceSize size = RHI::GetDeviceSize();
	SceneBuffer = std::make_shared<Texture>();
	RHI::CreateTexture(
		SceneBuffer.get(),
		L"SceneBuffer",
		size.Width,size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	halfSceneBuffer = std::make_shared<Texture>();
	RHI::CreateTexture(
		halfSceneBuffer.get(),
		L"halfSceneBuffer",
		size.Width / 2,size.Height / 2,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	quaterSceneBuffer1 = std::make_shared<Texture>();
	RHI::CreateTexture(
		quaterSceneBuffer1.get(),
		L"quaterSceneBuffer1",
		size.Width / 4,size.Height / 4,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	quaterSceneBuffer2 = std::make_shared<Texture>();
	RHI::CreateTexture(
		quaterSceneBuffer2.get(),
		L"quaterSceneBuffer2",
		size.Width / 4,size.Height / 4,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);
	IntermediateA = std::make_shared<Texture>();
	RHI::CreateTexture(
		IntermediateA.get(),
		L"IntermediateA",
		size.Width,size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	IntermediateB = std::make_shared<Texture>();
	RHI::CreateTexture(
		IntermediateB.get(),
		L"IntermediateB",
		size.Width,size.Height,
		defaultTextureFormat,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0
	);

	SSAOTexture = std::make_shared<Texture>();
	RHI::CreateTexture(
		SSAOTexture.get(),
		L"SSAOTexture",
		size.Width / 2,size.Height / 2,
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
		TonemapPass,
		BuiltIn_ToneMapping_PS_ByteCode,
		sizeof(BuiltIn_ToneMapping_PS_ByteCode)
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

	RHI::CreatePixelShader(
		ScreenSpaceAmbienceOcclusion,
		BuiltIn_SSAO_PS_ByteCode,
		sizeof(BuiltIn_SSAO_PS_ByteCode)
	);


	RHI::CreatePixelShader(
		EdgeBlur,
		BuiltIn_EdgeBlur_ByteCode,
		sizeof(BuiltIn_EdgeBlur_ByteCode)
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

void GraphicsEngine::UpdateSettings()
{
	myGraphicSettingsBuffer.Data.GSB_ToneMap = myGraphicSettings.Tonemaptype;
	myGraphicSettingsBuffer.Data.GSB_AO_intensity = 0.35f;
	myGraphicSettingsBuffer.Data.GSB_AO_scale = 0.05f;
	myGraphicSettingsBuffer.Data.GSB_AO_bias = 0.5f;
	myGraphicSettingsBuffer.Data.GSB_AO_radius = 0.002f;
	myGraphicSettingsBuffer.Data.GSB_AO_offset = 0.707f;
	RHI::SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER,REG_GraphicSettingsBuffer,myGraphicSettingsBuffer);
	RHI::UpdateConstantBufferData(myGraphicSettingsBuffer);
}


void GraphicsEngine::BeginFrame()
{
	myCamera = GameObjectManager::Get().GetCamera().TryGetComponent<cCamera>();
	if (!myCamera)
	{
		Logger::Err("No camera in scene. No render is possible");
	}
	UpdateSettings();

	// Here we should initialize our frame and clean up from the last one.  
	RHI::ClearRenderTarget(myBackBuffer.get(),myBackgroundColor);
	RHI::ClearDepthStencil(myDepthBuffer.get());
	RHI::ClearRenderTarget(SceneBuffer.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(halfSceneBuffer.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer1.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer2.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateA.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateB.get(),{ 0.0f,0.0f,0.0f,0.0f });
	myG_Buffer.ClearTargets();
	RHI::SetBlendState(nullptr);


	auto commandAllocator = GPU::m_Allocator[GPU::m_FrameIndex];
	auto backBuffer = GPU::m_renderTargets[GPU::m_FrameIndex];

	commandAllocator->Reset();
	GPU::m_CommandList->Reset(commandAllocator.Get(),nullptr);


	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET);

	GPU::m_CommandList->ResourceBarrier(1,&barrier);

	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(GPU::m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),GPU::m_FrameIndex,GPU::m_RtvDescriptorSize);
	GPU::m_CommandList->ClearRenderTargetView(rtv,clearColor,0,nullptr);


}

void GraphicsEngine::RenderFrame(float aDeltaTime,double aTotalTime)
{
	OPTICK_EVENT();
	aDeltaTime; aTotalTime;
	RHI::SetVertexShader(myVertexShader);

	OPTICK_EVENT("Gbuffer");
	RHI::BeginEvent(L"Start writing to gbuffer");
	myCamera->SetCameraToFrameBuffer();
	myG_Buffer.SetWriteTargetToBuffer(); //Let all write to textures
	OPTICK_EVENT("Deferred");
	DeferredCommandList.Execute();
	OPTICK_EVENT("Instanced Deferred");
	myInstanceRenderer.Execute(false);
	RHI::SetRenderTarget(nullptr,nullptr);
	myG_Buffer.UnsetResources();
	RHI::EndEvent();

	//decals
	//if picking check
	//SSAO
	//Do ambience pass? Clarit
	//Render all lights
	OPTICK_EVENT("SSAO");
	RHI::BeginEvent(L"SSAO");
	myCamera->SetCameraToFrameBuffer();
	GfxCmd_SSAO().ExecuteAndDestroy();
	RHI::EndEvent();

	////Render shadowmaps
	OPTICK_EVENT("ShadowMaps");
	RHI::BeginEvent(L"ShadowMaps");
	myShadowRenderer.Execute();
	RHI::EndEvent();

	OPTICK_EVENT("Lightning");
	RHI::BeginEvent(L"Lightning");
	myCamera->SetCameraToFrameBuffer();
	GfxCmd_SetRenderTarget(SceneBuffer.get(),nullptr).ExecuteAndDestroy();
	GfxCmd_SetLightBuffer().ExecuteAndDestroy(); //REFACTOR Change name to fit purpose
	RHI::EndEvent();
	// //Forward pass for light
	//Forbidden


	//Particles
	OPTICK_EVENT("Particles")
		RHI::BeginEvent(L"Particles");
	RHI::SetBlendState(GraphicsEngine::Get().GetAdditiveBlendState());
	GfxCmd_SetRenderTarget(SceneBuffer.get(),myDepthBuffer.get()).ExecuteAndDestroy();
	myParticleRenderer.Execute();
	RHI::EndEvent();

	//Post processing
	OPTICK_EVENT("Postpro")
		RHI::BeginEvent(L"PostPro");
	RHI::SetBlendState(nullptr);
	GfxCmd_LuminancePass().ExecuteAndDestroy(); // Render to IntermediateA
	GfxCmd_GaussianBlur().ExecuteAndDestroy();
	GfxCmd_Bloom().ExecuteAndDestroy();
	GfxCmd_ToneMapPass().ExecuteAndDestroy(); // Render: BackBuffer Read: REG_Target01
	RHI::EndEvent();

	//Debug layers
	OPTICK_EVENT("DebugLayers")
		RHI::BeginEvent(L"DebugLayers");
	myCamera->SetCameraToFrameBuffer();
	GfxCmd_DebugLayer().ExecuteAndDestroy();
#ifdef  _DEBUGDRAW
	if (myGraphicSettings.DebugRenderer_Active)
	{
		RHI::SetRenderTarget(GraphicsEngine::Get().GetTargetTextures(eRenderTargets::BackBuffer).get(),GraphicsEngine::Get().GetTargetTextures(eRenderTargets::DepthBuffer).get());
		DebugDrawer::Get().Render();
	}
	OverlayCommandList.Execute();
#endif //  _DEBUGDRAW
	RHI::EndEvent();
}

void GraphicsEngine::RenderTextureTo(eRenderTargets from,eRenderTargets to)  const
{
	const Texture* texture1 = GraphicsEngine::Get().GetTargetTextures(from).get();
	const Texture* texture2 = GraphicsEngine::Get().GetTargetTextures(to).get();

	RHI::SetRenderTarget(texture2,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,texture1);
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,nullptr);
}

void GraphicsEngine::EndFrame()
{
	OPTICK_EVENT()
		// We finish our frame here and present it on screen. 
		RHI::Present(0);
	OPTICK_EVENT("ResetShadowList")
		myShadowRenderer.ResetShadowList();
	OPTICK_EVENT("DeferredCommandList")
		DeferredCommandList.Reset();
	OPTICK_EVENT("OverlayCommandList")
		OverlayCommandList.Reset();
	OPTICK_EVENT("myInstanceRenderer")
		myInstanceRenderer.Clear();
	OPTICK_EVENT("ClearedEndFrame")
}

