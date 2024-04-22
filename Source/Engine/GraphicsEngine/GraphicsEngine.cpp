#include "GraphicsEngine.pch.h"

#include <d3dcompiler.h>
#include <DirectX/directx/d3dx12_pipeline_state_stream.h>
#include <DirectX/directx/d3dx12_root_signature.h>
#include <DirectX/XTK/DDSTextureLoader.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>
#include "DirectX/Shipyard/CommandList.h"

#include <Objects/DataObjects/Default_C.h>
#include <Objects/DataObjects/Default_FX.h>
#include <Objects/DataObjects/Default_M.h>
#include <Objects/DataObjects/Default_N.h>

#include "DirectX/Shipyard/GPU.h"
#include "DirectX/Shipyard/Helpers.h"
#include "DirectX/Shipyard/PSO.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Tools/ImGui/ImGui/backends/imgui_impl_dx12.h"
#include "Tools/Utilities/Input/InputHandler.hpp"


bool GraphicsEngine::Initialize(HWND windowHandle,bool enableDeviceDebug)
{
#ifdef _DEBUG
	//try
	//{
#endif 
	myWindowHandle = windowHandle;
	myBackBuffer = std::make_unique<Texture>();
	myDepthBuffer = std::make_unique<Texture>();
	if (!GPU::Initialize(myWindowHandle,
		enableDeviceDebug,
		myBackBuffer,
		myDepthBuffer,
		Window::Width(),Window::Height()))
	{
		Logger::Err("Failed to initialize the DX12 GPU!");
		return false;
	}

	/*if (!RHI::Initialize(myWindowHandle,
		enableDeviceDebug,
		myBackBuffer.get(),
		myDepthBuffer.get()))
	{
		Logger::Err("Failed to initialize the RHI!");
		return false;
	}*/

	m_StateCache = std::make_unique<PSOCache>();
	m_StateCache->InitAllStates();

	SetupDefaultVariables();
	SetupSpace3();
	SetupParticleShaders();
	SetupPostProcessing();
	SetupBlendStates();
	SetupDebugDrawline();



	/*
	myG_Buffer.Init();
	myShadowRenderer.Init();
	myParticleRenderer.Init();
	RHI::SetDepthState(DepthState::DS_Reversed);*/

#ifdef _DEBUG
	//}
	//catch (const std::exception& e)
	//{
	//	GPU::UnInitialize();
	//	ComPtr<ID3D12DeviceRemovedExtendedData> pDred;
	//	SUCCEEDED(GPU::m_Device->QueryInterface(IID_PPV_ARGS(&pDred)));
	//
	//	D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT DredAutoBreadcrumbsOutput;
	//	D3D12_DRED_PAGE_FAULT_OUTPUT DredPageFaultOutput;
	//	SUCCEEDED(pDred->GetAutoBreadcrumbsOutput(&DredAutoBreadcrumbsOutput));
	//	SUCCEEDED(pDred->GetPageFaultAllocationOutput(&DredPageFaultOutput));
	//	const D3D12_AUTO_BREADCRUMB_NODE* pBreadcrumbs = DredAutoBreadcrumbsOutput.pHeadAutoBreadcrumbNode;
	//
	//	Logger::LogException(e);
	//	system("pause");
	//	exit(-1);
	//}
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
	D3D12_SAMPLER_DESC pointSamplerDesc = {};
	pointSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	pointSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pointSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pointSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;


	////Particle
	//AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/DefaultParticle_P.dds",defaultParticleTexture);
	//defaultParticleTexture->SetTextureType(eTextureType::ParticleMap);

	//NOISE

	//if (!RHI::CreateSamplerState(myPointSampleState,pointSamplerDesc))
	//{
	//	Logger::Log("Sampler state created");
	//	assert(false);
	//}


	//RHI::SetSamplerState(myPointSampleState,REG_PointSampler); 
	//AssetManager::Get().ForceLoadAsset<TextureHolder>(L"Textures/Default/NoiseTable.dds",NoiseTable);
	//RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Noise_Texture,NoiseTable->GetRawTexture().get()); //Is there guarantee that this holds?


	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",defaultVS);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso",defaultPS);
	AssetManager::Get().ForceLoadAsset<Material>("Materials/Default.json",defaultMaterial);
	defaultMaterial->SetShader(defaultVS,defaultPS);


	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultTile.dds",defaultTexture);
	defaultTexture->SetTextureType(eTextureType::ColorMap);
	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultNormal.dds",defaultNormalTexture);
	defaultNormalTexture->SetTextureType(eTextureType::NormalMap);
	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultMaterial.dds",defaultMatTexture);
	defaultMatTexture->SetTextureType(eTextureType::MaterialMap);
	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/Default/DefaultEffect.dds",defaultEffectTexture);
	defaultEffectTexture->SetTextureType(eTextureType::EffectMap);

	AssetManager::Get().ForceLoadAsset<Mesh>("default.fbx",defaultMesh);
}

void GraphicsEngine::SetupBlendStates()
{
	/*D3D11_BLEND_DESC blendDesc = {};
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
	}*/
}

void GraphicsEngine::SetupSpace3()
{

	auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue->GetCommandList(L"RenderFrame");
	auto graphicCommandList = commandList->GetGraphicsCommandList();


	//Light
	AssetManager::Get().ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds",defaultCubeMap);
	defaultCubeMap->SetTextureType(eTextureType::CubeMap);

	Vector2ui size = { 512,512 };
	BRDLookUpTable = std::make_shared<Texture>();
	BRDLookUpTable->AllocateTexture(
		size,
		L"brdfLUT",
		DXGI_FORMAT_R16G16_FLOAT
	);
	BRDLookUpTable->SetView(ViewType::SRV);
	commandList->SetRenderTargets(1,BRDLookUpTable.get(),nullptr);
	PSO brdfPSO = PSO::CreatePSO("Shaders/ScreenspaceQuad_VS.cso","Shaders/brdfLUT_PS.cso",1,DXGI_FORMAT_R16G16_FLOAT);

	D3D12_VIEWPORT viewPort = { 0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH };
	D3D12_RECT rect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };

	graphicCommandList->RSSetViewports(1,&viewPort);
	graphicCommandList->RSSetScissorRects(1,&rect);

	const auto& rootSignature = PSOCache::m_RootSignature->GetRootSignature();
	graphicCommandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->TrackResource(rootSignature);

	graphicCommandList->SetPipelineState(brdfPSO.GetPipelineState().Get());

	graphicCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicCommandList->IASetVertexBuffers(0,1,nullptr);
	graphicCommandList->IASetIndexBuffer(nullptr);
	graphicCommandList->DrawInstanced(6,1,0,0);

	auto fence = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fence);
}

void GraphicsEngine::SetupPostProcessing()
{
	//D3D11_SAMPLER_DESC normalDepthSampler = {};
	//normalDepthSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	//normalDepthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	//normalDepthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	//normalDepthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	//if (!RHI::CreateSamplerState(myNormalDepthSampleState,normalDepthSampler))
	//{
	//	Logger::Log("Sampler state created");
	//	assert(false);
	//}

	//RHI::SetSamplerState(myNormalDepthSampleState,REG_normalDepthSampler);

	//RHI::DeviceSize size = RHI::GetDeviceSize();
	//SceneBuffer = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	SceneBuffer.get(),
	//	L"SceneBuffer",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	//halfSceneBuffer = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	halfSceneBuffer.get(),
	//	L"halfSceneBuffer",
	//	size.Width / 2,size.Height / 2,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	//quaterSceneBuffer1 = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	quaterSceneBuffer1.get(),
	//	L"quaterSceneBuffer1",
	//	size.Width / 4,size.Height / 4,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	//quaterSceneBuffer2 = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	quaterSceneBuffer2.get(),
	//	L"quaterSceneBuffer2",
	//	size.Width / 4,size.Height / 4,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	//IntermediateA = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	IntermediateA.get(),
	//	L"IntermediateA",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	//IntermediateB = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	IntermediateB.get(),
	//	L"IntermediateB",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	//SSAOTexture = std::make_shared<Texture>();
	//RHI::CreateTexture(
	//	SSAOTexture.get(),
	//	L"SSAOTexture",
	//	size.Width / 2,size.Height / 2,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	//RHI::CreatePixelShader(
	//	luminancePass,
	//	BuiltIn_LuminancePass_PS_ByteCode,
	//	sizeof(BuiltIn_LuminancePass_PS_ByteCode)
	//);
	//RHI::CreatePixelShader(
	//	TonemapPass,
	//	BuiltIn_ToneMapping_PS_ByteCode,
	//	sizeof(BuiltIn_ToneMapping_PS_ByteCode)
	//);

	//RHI::CreatePixelShader(
	//	copyShader,
	//	BuiltIn_CopyPixels_PS_ByteCode,
	//	sizeof(BuiltIn_CopyPixels_PS_ByteCode)
	//);

	//RHI::CreatePixelShader(
	//	gaussShader,
	//	BuiltIn_GaussianBlur_PS_ByteCode,
	//	sizeof(BuiltIn_GaussianBlur_PS_ByteCode)
	//);

	//RHI::CreatePixelShader(
	//	bloomShader,
	//	BuiltIn_Bloom_PS_ByteCode,
	//	sizeof(BuiltIn_Bloom_PS_ByteCode)
	//);

	//RHI::CreatePixelShader(
	//	ScreenSpaceAmbienceOcclusion,
	//	BuiltIn_SSAO_PS_ByteCode,
	//	sizeof(BuiltIn_SSAO_PS_ByteCode)
	//);


	//RHI::CreatePixelShader(
	//	EdgeBlur,
	//	BuiltIn_EdgeBlur_ByteCode,
	//	sizeof(BuiltIn_EdgeBlur_ByteCode)
	//);

}

void GraphicsEngine::SetupParticleShaders()
{
	/*RHI::CreateVertexShaderAndInputLayout(
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
	);*/
}

void GraphicsEngine::UpdateSettings()
{
	myGraphicSettingsBuffer.GSB_ToneMap = myGraphicSettings.Tonemaptype;
	myGraphicSettingsBuffer.GSB_AO_intensity = 0.35f;
	myGraphicSettingsBuffer.GSB_AO_scale = 0.05f;
	myGraphicSettingsBuffer.GSB_AO_bias = 0.5f;
	myGraphicSettingsBuffer.GSB_AO_radius = 0.002f;
	myGraphicSettingsBuffer.GSB_AO_offset = 0.707f;
	/*
	RHI::SetConstantBuffer(PIPELINE_STAGE_PIXEL_SHADER,REG_GraphicSettingsBuffer,myGraphicSettingsBuffer);
	RHI::UpdateConstantBufferData(myGraphicSettingsBuffer);
	*/
}

void GraphicsEngine::Update()
{
	myCamera = GameObjectManager::Get().GetCamera().TryGetComponent<cCamera>();
	if (!myCamera)
	{
		Logger::Err("No camera in scene. No render is possible");
	}









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
	/*RHI::ClearRenderTarget(myBackBuffer.get(),myBackgroundColor);
	RHI::ClearDepthStencil(myDepthBuffer.get());
	RHI::ClearRenderTarget(SceneBuffer.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(halfSceneBuffer.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer1.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(quaterSceneBuffer2.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateA.get(),{ 0.0f,0.0f,0.0f,0.0f });
	RHI::ClearRenderTarget(IntermediateB.get(),{ 0.0f,0.0f,0.0f,0.0f });
	myG_Buffer.ClearTargets();
	RHI::SetBlendState(nullptr);*/




}

void GraphicsEngine::RenderFrame(float aDeltaTime,double aTotalTime)
{
	OPTICK_EVENT();
	aDeltaTime; aTotalTime;

	auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue->GetCommandList(L"RenderFrame");
	auto graphicCommandList = commandList->GetGraphicsCommandList();
	auto chain = GPU::m_Swapchain->m_SwapChain;

	//const UINT currentBackBufferIndex = chain->GetCurrentBackBufferIndex();
	const auto* backBuffer = GPU::GetCurrentBackBuffer();
	const auto rtv = GPU::GetCurrentRenderTargetView();
	const auto dsv = GPU::m_DepthBuffer->GetHandle(ViewType::DSV).cpuPtr;
	commandList->TransitionBarrier(backBuffer->GetResource(),D3D12_RESOURCE_STATE_RENDER_TARGET);
	Vector4f clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	GPU::ClearRTV(*commandList.get(),rtv,clearColor);
	GPU::ClearDepth(*commandList.get(),GPU::m_DepthBuffer->GetHandle(ViewType::DSV).cpuPtr);

	const auto& rootSignature = PSOCache::m_RootSignature->GetRootSignature();
	graphicCommandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->TrackResource(rootSignature);

	ID3D12DescriptorHeap* heaps[] =
	{
		GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->Heap(),
		GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_SAMPLER)]->Heap()
	};
	graphicCommandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)),heaps);

	{
		LightBuffer lightBuffer = EnvironmentLightPSO::CreateLightBuffer();
		const auto& alloc = GPU::m_GraphicsMemory->AllocateConstant<LightBuffer>(lightBuffer);
		graphicCommandList->SetGraphicsRootConstantBufferView(REG_LightBuffer,alloc.GpuAddress());

		auto frameBuffer = myCamera->GetFrameBuffer();
		const auto& alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
		graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::frameBuffer,alloc0.GpuAddress());

		const auto cubeMap = defaultCubeMap->GetRawTexture().get();
		commandList->SetDescriptorTable(eRootBindings::PermanentTextures,cubeMap);
		commandList->TrackResource(cubeMap->GetResource());

		graphicCommandList->SetGraphicsRootDescriptorTable(eRootBindings::Textures,GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetFirstGpuHandle());
		graphicCommandList->SetGraphicsRootDescriptorTable(eRootBindings::MeshBuffer,GPU::m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->GetFirstGpuHandle());
	}


	static auto& list = GameObjectManager::Get().GetAllComponents<cMeshRenderer>();
	ShadowMapperPSO::WriteShadows(commandList,list);
	commandList->FlushResourceBarriers();

	Texture* gBufferTextures;
	unsigned bufferCount = 0;
	{

		graphicCommandList->RSSetViewports(1,&GPU::m_Viewport);
		graphicCommandList->RSSetScissorRects(1,&GPU::m_ScissorRect);

		const auto& gbufferPSO = PSOCache::GetState(PSOCache::ePipelineStateID::GBuffer);
		bufferCount = gbufferPSO->GetRenderTargetAmounts();
		gBufferTextures = gbufferPSO->GetRenderTargets();
		GPU::ClearRTV(*commandList.get(),gBufferTextures,bufferCount);
		commandList->SetRenderTargets(bufferCount,gBufferTextures,GPU::m_DepthBuffer.get());

		const auto& pipelineState = gbufferPSO->GetPipelineState().Get();
		graphicCommandList->SetPipelineState(pipelineState);
		commandList->TrackResource(pipelineState);

		auto frameBuffer = myCamera->GetFrameBuffer();
		const auto& alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
		graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::frameBuffer,alloc0.GpuAddress());
	}

	int vertCount = 0;
	for (auto& meshRenderer : list)
	{
		const auto& transform = meshRenderer.GetComponent<Transform>();
		for (auto& element : meshRenderer.GetElements())
		{
			ObjectBuffer objectBuffer;
			objectBuffer.myTransform = transform.GetRawTransform();
			objectBuffer.MaxExtents = Vector3f(1,1,1);
			objectBuffer.MinExtents = -Vector3f(1,1,1);
			objectBuffer.hasBone = false;
			objectBuffer.isInstanced = false;

			const auto& alloc1 = GPU::m_GraphicsMemory->AllocateConstant<ObjectBuffer>(objectBuffer);
			graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::objectBuffer,alloc1.GpuAddress());
			vertCount += element.VertexBuffer.GetVertexCount();
			GPU::ConfigureInputAssembler(*commandList,D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,element.VertexBuffer,element.IndexResource);

			const unsigned materialIndex = element.MaterialIndex;
			MaterialBuffer materialBuffer;

			for (int i = 0; i < static_cast<int>(eTextureType::EffectMap) + 1; i++)
			{
				if (auto textureAsset = meshRenderer.GetTexture(static_cast<eTextureType>(i),materialIndex))
				{
					auto tex = textureAsset->GetRawTexture();
					tex->SetView(ViewType::SRV);
					commandList->TrackResource(tex->GetResource());

					switch (static_cast<eTextureType>(i))
					{
					case eTextureType::ColorMap:
						materialBuffer.albedoTexture = tex->GetHeapOffset();
						break;
					case eTextureType::NormalMap:
						materialBuffer.normalTexture = tex->GetHeapOffset();
						break;
					case eTextureType::MaterialMap:
						materialBuffer.materialTexture = tex->GetHeapOffset();
						break;
					case eTextureType::EffectMap:
						materialBuffer.emissiveTexture = tex->GetHeapOffset();
						break;
					default:
						break;
					}
				}
				else
				{
					switch (static_cast<eTextureType>(i))
					{
					case eTextureType::ColorMap:
						materialBuffer.albedoTexture = defaultTexture->GetRawTexture()->GetHeapOffset();
						break;
					case eTextureType::NormalMap:
						materialBuffer.normalTexture = defaultNormalTexture->GetRawTexture()->GetHeapOffset();
						break;
					case eTextureType::MaterialMap:
						materialBuffer.materialTexture = defaultMatTexture->GetRawTexture()->GetHeapOffset();
						break;
					case eTextureType::EffectMap:
						materialBuffer.emissiveTexture = defaultEffectTexture->GetRawTexture()->GetHeapOffset();
						break;
					default:
						break;
					}
				}
			}

			materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
			materialBuffer.vertexOffset = 0; //vertex offset is part of drawcall, if i ever use this i need to set it here

			const auto& alloc2 = GPU::m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
			graphicCommandList->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,alloc2.GpuAddress());

			graphicCommandList->DrawIndexedInstanced(element.IndexResource.GetIndexCount(),1,0,0,0);
		}
	}



	const auto& enviromentLight = PSOCache::GetState(PSOCache::ePipelineStateID::DeferredLighting);
	{
		GPU::ClearRTV(*commandList.get(),enviromentLight->GetRenderTargets(),enviromentLight->GetRenderTargetAmounts());
		commandList->SetRenderTargets(enviromentLight->GetRenderTargetAmounts(),enviromentLight->GetRenderTargets(),nullptr);

		const auto& pipelineState = enviromentLight->GetPipelineState().Get();
		graphicCommandList->SetPipelineState(pipelineState);
		commandList->TrackResource(pipelineState);

		for (unsigned i = 0; i < bufferCount; i++)
		{
			gBufferTextures[i].SetView(ViewType::SRV);
			commandList->TransitionBarrier(gBufferTextures[i],D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->TrackResource(gBufferTextures[i]);
		}
		commandList->FlushResourceBarriers();
		commandList->SetDescriptorTable(eRootBindings::GbufferPasses,gBufferTextures);
		graphicCommandList->IASetVertexBuffers(0,1,nullptr);
		graphicCommandList->IASetIndexBuffer(nullptr);
		graphicCommandList->DrawInstanced(6,1,0,0);
	}

	{
		const auto& toneMapper = PSOCache::GetState(PSOCache::ePipelineStateID::ToneMap);

		const auto& pipelineState = toneMapper->GetPipelineState().Get();
		graphicCommandList->SetPipelineState(pipelineState);
		commandList->TrackResource(pipelineState);

		auto* renderTargets = enviromentLight->GetRenderTargets();
		commandList->SetDescriptorTable(TargetTexture,renderTargets);
		commandList->FlushResourceBarriers();
		commandList->SetRenderTargets(1,GPU::GetCurrentBackBuffer(),nullptr);


		graphicCommandList->IASetVertexBuffers(0,1,nullptr);
		graphicCommandList->IASetIndexBuffer(nullptr);
		graphicCommandList->DrawInstanced(6,1,0,0);
	}



	ImGui::Render();

	ID3D12DescriptorHeap* ImGuiHeap[] =
	{
		GPU::m_ImGui_Heap->Heap(),nullptr
	};

	graphicCommandList->SetDescriptorHeaps(1,ImGuiHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(),graphicCommandList.Get());

	commandQueue->ExecuteCommandList(commandList);














}

void GraphicsEngine::RenderTextureTo(eRenderTargets from,eRenderTargets to)  const
{
	from; to;

	//const Texture* texture1 = GraphicsEngine::Get().GetTargetTextures(from).get();
	//const Texture* texture2 = GraphicsEngine::Get().GetTargetTextures(to).get();

	/*RHI::SetRenderTarget(texture2,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,texture1);
	RHI::ConfigureInputAssembler(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,
		nullptr,
		0,
		nullptr
	);
	RHI::Draw(4);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_Target0,nullptr);*/
}

void GraphicsEngine::EndFrame()
{
	OPTICK_EVENT();
	auto commandQueue = GPU::GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto commandList = commandQueue->GetCommandList();

	const auto* backBuffer = GPU::GetCurrentBackBuffer();
	commandList->TransitionBarrier(backBuffer->GetResource(),D3D12_RESOURCE_STATE_PRESENT);
	commandQueue->ExecuteCommandList(commandList);

	Helpers::ThrowIfFailed(GPU::m_Swapchain->m_SwapChain->Present(DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD,DXGI_PRESENT_ALLOW_TEARING));
	GPU::m_FenceValues[GPU::m_FrameIndex] = commandQueue->Signal();
	GPU::m_FrameIndex = GPU::m_Swapchain->m_SwapChain->GetCurrentBackBufferIndex();

	GPU::m_GraphicsMemory->Commit(commandQueue->GetCommandQueue().Get());
	commandQueue->WaitForFenceValue(GPU::m_FenceValues[GPU::m_FrameIndex]);
}

FORCEINLINE std::shared_ptr<Texture> GraphicsEngine::GetTargetTextures(eRenderTargets type) const
{
	switch (type)
	{
	case::eRenderTargets::BackBuffer:
		return myBackBuffer;
	case::eRenderTargets::DepthBuffer:
		return myDepthBuffer;
	case::eRenderTargets::SceneBuffer:
		return SceneBuffer;
	case::eRenderTargets::halfSceneBuffer:
		return halfSceneBuffer;
	case::eRenderTargets::quaterSceneBuffer1:
		return quaterSceneBuffer1;
	case::eRenderTargets::quaterSceneBuffer2:
		return quaterSceneBuffer2;
	case::eRenderTargets::IntermediateA:
		return IntermediateA;
	case::eRenderTargets::IntermediateB:
		return IntermediateB;
	case::eRenderTargets::SSAO:
		return SSAOTexture;
	case::eRenderTargets::NoiseTexture:
		return NoiseTable->GetRawTexture();
	default:
		return nullptr;
	}
}