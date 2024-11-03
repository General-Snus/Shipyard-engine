#include "GraphicsEngine.pch.h"

#include "DirectX/DX12/Graphics/CommandList.h"
#include <DirectX/DirectXTex/DirectXTex.h>

#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/ComponentSystem/Components/CameraComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/LightComponent.h>
#include <Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/MeshAsset.h>
#include <Engine/AssetManager/Objects/BaseAssets/TextureAsset.h>

#include "DirectX/DX12/Graphics/GPU.h"
#include "DirectX/DX12/Graphics/Helpers.h"
#include "DirectX/DX12/Graphics/PSO.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Engine/PersistentSystems/Scene.h"

#include "Rendering/Passes.h"
#include "Tools/ImGui/ImGuizmo.h"
#include "Tools/ImGui/backends/imgui_impl_dx12.h"
#include "Tools/ImGui/imgui_internal.h"
#include "Tools/Utilities/Input/Input.hpp"
#include <Editor/Editor/Windows/EditorWindows/Viewport.h>
#include <Rendering/Buffers/ObjectBuffer.h>

bool GraphicsEngine::Initialize(HWND windowHandle, bool enableDeviceDebug)
{
	enableDeviceDebug;
	if (!GPUInstance.Initialize(windowHandle, true, WindowInstance.Width(), WindowInstance.Height()))
	{
		Logger.Err("Failed to initialize the DX12 GPU!");
		return false;
	}

	m_Cache = std::make_shared<PSOCache>();
	m_Cache->InitRootSignature();
	m_Cache->InitAllStates();

	SetupDefaultVariables();
	SetupSpace3();
	SetupParticleShaders();
	SetupPostProcessing();
	SetupBlendStates();
	SetupDebugDrawline();

	InitializeCustomRenderScene();

	const auto &gBufferTextures = m_Cache->GetState(PSOCache::ePipelineStateID::GBuffer)->RenderTargets();

	const size_t dataSize = gBufferTextures[5].GetWidth() * gBufferTextures[5].GetHeight() * sizeof(float) * 2;
	BufferForPicking = new uint32_t[dataSize];
	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	OPTICK_GPU_INIT_D3D12(GPUInstance.m_Device.Get(), commandQueue->GetCommandQueue().GetAddressOf(), 1);
	return true;
}

void GraphicsEngine::InitializeCustomRenderScene()
{
	newScene = std::make_shared<Scene>("Custom renderer Scene");
	{
		GameObject worldRoot = GameObject::Create("WordRoot", newScene);
		Transform &transform = worldRoot.AddComponent<Transform>();
		transform.SetRotation(80, 0, 0);
		transform.SetPosition(0, 5, 0);
		Light &pLight = worldRoot.AddComponent<Light>(eLightType::Directional);
		pLight.SetColor(Vector3f(1, 1, 1));
		pLight.SetPower(2.0f);
		pLight.BindDirectionToTransform(true);
	}

	const auto res = Vector2f(1920.f, 1080.f);
	{
		GameObject camera = GameObject::Create("Camera", newScene);
		auto &cameraComponent = camera.AddComponent<Camera>();
		cameraComponent.SetResolution(res);

		newScene->GetGOM().SetLastGOAsCamera();
		cameraComponent.SetActive(true);
	}

	{
		auto renderObject = GameObject::Create("RenderMesh", newScene);
		renderObject.AddComponent<Transform>();
		renderObject.AddComponent<MeshRenderer>();
		newScene->GetGOM().SetLastGOAsPlayer();
	}
}

const PSOCache &GraphicsEngine::GetPSOCache() const
{
	return *m_Cache;
}

bool GraphicsEngine::ResizeBuffers(Vector2ui resolution)
{
	GPUInstance.Resize(resolution);
	m_Cache->InitAllStates();

	return true; // TODO Makes sense to check if the resolution is supporded by the monitor
}

uint32_t GraphicsEngine::ReadPickingData(Vector2ui position)
{
	const int pos = position.x * position.y;

	return (unsigned)pos;
}

bool GraphicsEngine::SetupDebugDrawline()
{
	DebugDrawer::Get().Initialize();
	DebugDrawer::Get().AddDebugGrid({0.f, 0.0f, 0.f}, 1000, 10, {1.0f, 1.0f, 1.0f});
	return true;
}
void GraphicsEngine::SetupDefaultVariables()
{

	////Particle
	EngineResources.ForceLoadAsset<TextureHolder>(L"Textures/Default/DefaultParticle_P.dds", defaultParticleTexture);
	defaultParticleTexture->SetTextureType(eTextureType::ParticleMap);

	EngineResources.ForceLoadAsset<TextureHolder>(L"Textures/Default/NoiseTable.dds", NoiseTable);

	EngineResources.ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso", defaultVS);
	EngineResources.ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso", defaultPS);
	EngineResources.ForceLoadAsset<Material>("Materials/Default.json", defaultMaterial);
	defaultMaterial->SetShader(defaultVS, defaultPS);

	EngineResources.ForceLoadAsset<TextureHolder>("Textures/Default/DefaultTile.dds", defaultTexture);
	defaultTexture->SetTextureType(eTextureType::ColorMap);
	EngineResources.ForceLoadAsset<TextureHolder>("Textures/Default/DefaultNormal.dds", defaultNormalTexture);
	defaultNormalTexture->SetTextureType(eTextureType::NormalMap);
	EngineResources.ForceLoadAsset<TextureHolder>("Textures/Default/DefaultMaterial.dds", defaultMatTexture);
	defaultMatTexture->SetTextureType(eTextureType::MaterialMap);
	EngineResources.ForceLoadAsset<TextureHolder>("Textures/Default/DefaultEffect.dds", defaultEffectTexture);
	defaultEffectTexture->SetTextureType(eTextureType::EffectMap);

	EngineResources.ForceLoadAsset<Mesh>("default.fbx", defaultMesh);
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
	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	const auto commandList = commandQueue->GetCommandList(L"RenderFrame");
	const auto graphicCommandList = commandList->GetGraphicsCommandList();

	// Light
	EngineResources.ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds", defaultCubeMap);
	defaultCubeMap->SetTextureType(eTextureType::CubeMap);

	const Vector2ui size = {512, 512};
	BRDLookUpTable = std::make_shared<Texture>();
	BRDLookUpTable->AllocateTexture(size, L"brdfLUT", Vector4f(), DXGI_FORMAT_R16G16_FLOAT);
	BRDLookUpTable->SetView(ViewType::SRV);
	commandList->SetRenderTargets(1, BRDLookUpTable.get(), nullptr);

	constexpr std::array rt = {DXGI_FORMAT_R16G16_FLOAT};
	const auto brdfPSO = m_Cache->CreatePSO("Shaders/ScreenspaceQuad_VS.cso", "Shaders/brdfLUT_PS.cso", rt);

	const D3D12_VIEWPORT viewPort = {
		0.0f, 0.0f, static_cast<float>(size.x), static_cast<float>(size.y), D3D12_MIN_DEPTH, D3D12_MAX_DEPTH};
	const D3D12_RECT rect = {0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y)};

	graphicCommandList->RSSetViewports(1, &viewPort);
	graphicCommandList->RSSetScissorRects(1, &rect);

	const auto &rootSignature = m_Cache->m_RootSignature->GetRootSignature();
	graphicCommandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->TrackResource(rootSignature);

	graphicCommandList->SetPipelineState(brdfPSO->GetPipelineState().Get());

	graphicCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicCommandList->IASetVertexBuffers(0, 1, nullptr);
	graphicCommandList->IASetIndexBuffer(nullptr);
	graphicCommandList->DrawInstanced(6, 1, 0, 0);

	const auto fence = commandQueue->ExecuteCommandList(commandList);
	commandQueue->WaitForFenceValue(fence);
}
void GraphicsEngine::SetupPostProcessing()
{
	// D3D11_SAMPLER_DESC normalDepthSampler = {};
	// normalDepthSampler.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	// normalDepthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	// normalDepthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	// normalDepthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// if (!RHI::CreateSamplerState(myNormalDepthSampleState,normalDepthSampler))
	//{
	//	Logger.Log("Sampler state created");
	//	assert(false);
	// }

	// RHI::SetSamplerState(myNormalDepthSampleState,REG_normalDepthSampler);

	// RHI::DeviceSize size = RHI::GetDeviceSize();
	// SceneBuffer = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	SceneBuffer.get(),
	//	L"SceneBuffer",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	// halfSceneBuffer = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	halfSceneBuffer.get(),
	//	L"halfSceneBuffer",
	//	size.Width / 2,size.Height / 2,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	// quaterSceneBuffer1 = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	quaterSceneBuffer1.get(),
	//	L"quaterSceneBuffer1",
	//	size.Width / 4,size.Height / 4,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	// quaterSceneBuffer2 = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	quaterSceneBuffer2.get(),
	//	L"quaterSceneBuffer2",
	//	size.Width / 4,size.Height / 4,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);
	// IntermediateA = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	IntermediateA.get(),
	//	L"IntermediateA",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	// IntermediateB = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	IntermediateB.get(),
	//	L"IntermediateB",
	//	size.Width,size.Height,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
	//);

	// SSAOTexture = std::make_shared<Texture>();
	// RHI::CreateTexture(
	//	SSAOTexture.get(),
	//	L"SSAOTexture",
	//	size.Width / 2,size.Height / 2,
	//	defaultTextureFormat,
	//	D3D11_USAGE_DEFAULT,
	//	D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
	//	0
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
}

void GraphicsEngine::AddRenderJob(std::shared_ptr<Viewport> aViewport)
{
	m_CustomSceneRenderPasses.emplace_back(aViewport);
}

uint32_t GraphicsEngine::GetAmountOfRenderJob()
{
	return static_cast<uint32_t>(m_CustomSceneRenderPasses.size());
}

void GraphicsEngine::Render(std::vector<std::shared_ptr<Viewport>> renderViewPorts)
{
	OPTICK_EVENT();

	for (auto &viewport : m_CustomSceneRenderPasses)
	{
		renderViewPorts.emplace_back(viewport);
		viewport->Update();
	}

	BeginFrame();

	for (auto &viewport : renderViewPorts)
	{
		RenderFrame(*viewport, viewport->GetAttachedScene()->GetGOM());
	}
	EndFrame();

	for (auto &viewport : m_CustomSceneRenderPasses)
	{
		viewport->m_RenderTarget->isBeingLoaded = false;
		viewport->m_RenderTarget->isLoadedComplete = true;
	}
	m_CustomSceneRenderPasses.clear();
}

void GraphicsEngine::BeginFrame()
{
	OPTICK_EVENT();
	myCamera = Scene::ActiveManager().GetCamera().TryGetComponent<Camera>();
	if (!myCamera)
	{
		// Logger.Err("No camera in scene. No render is possible");
	}
	UpdateSettings();
}

uint64_t GraphicsEngine::RenderFrame(Viewport &renderViewPort, GameObjectManager &scene)
{
	if (renderViewPort.IsRenderReady())
	{
		OPTICK_EVENT();
		auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue->GetCommandList(L"RenderFrame");
		OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());

		OPTICK_GPU_EVENT("RenderFrame");
		PrepareBuffers(commandList, renderViewPort, scene);
		Passes::WriteShadows(*this, commandList, scene);
		commandList->FlushResourceBarriers();

		DeferredRenderingPass(commandList, renderViewPort, scene);
		EnvironmentLightPass(commandList);
		ToneMapperPass(commandList, renderViewPort.GetTarget());
		return commandQueue->ExecuteCommandList(commandList);
	}
	else
	{
		auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
		auto commandList = commandQueue->GetCommandList(L"RenderFrame");
		GPUInstance.ClearRTV(*commandList, renderViewPort.GetTarget());
		return commandQueue->ExecuteCommandList(commandList);
	}
}

void GraphicsEngine::EndFrame()
{
	OPTICK_GPU_EVENT("EndFrame");
	ImGuiPass();
	GPUInstance.Present();
}

void GraphicsEngine::PrepareBuffers(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort,
									GameObjectManager &scene)
{
	OPTICK_GPU_EVENT("PrepareBuffers");
	commandList->TransitionBarrier(GPUInstance.GetCurrentBackBuffer().GetResource(),
								   D3D12_RESOURCE_STATE_RENDER_TARGET);

	GPU::ClearRTV(*commandList.get(), GPUInstance.GetCurrentRenderTargetView());
	GPU::ClearDepth(*commandList.get(), GPUInstance.m_DepthBuffer->GetHandle(ViewType::DSV).cpuPtr);

	const auto &rootSignature = m_Cache->m_RootSignature->GetRootSignature();
	commandList->GetGraphicsCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	commandList->TrackResource(rootSignature);

	ID3D12DescriptorHeap *heaps[] = {
		GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->Heap(),
		GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_SAMPLER)]->Heap()};
	commandList->GetGraphicsCommandList()->SetDescriptorHeaps((UINT)std::size(heaps), heaps);

	{
		const LightBuffer lightBuffer = Passes::CreateLightBuffer(scene);
		const GraphicsResource &alloc = GPUInstance.m_GraphicsMemory->AllocateConstant<LightBuffer>(lightBuffer);
		commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(REG_LightBuffer, alloc.GpuAddress());

		const auto frameBuffer = renderViewPort.GetCamera().GetFrameBuffer();
		const auto &alloc0 = GPUInstance.m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
		commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView((int)eRootBindings::frameBuffer,
																				 alloc0.GpuAddress());

		const auto cubeMap = defaultCubeMap->GetRawTexture().get();
		commandList->SetDescriptorTable((int)eRootBindings::PermanentTextures, cubeMap);
		commandList->TrackResource(cubeMap->GetResource());

		commandList->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			(int)eRootBindings::Textures,
			GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]
				->GetFirstGpuHandle());
		commandList->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(
			(int)eRootBindings::MeshBuffer,
			GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]
				->GetFirstGpuHandle());
	}
}
void GraphicsEngine::DeferredRenderingPass(std::shared_ptr<CommandList> commandList, Viewport &renderViewPort,
										   GameObjectManager &scene)
{
	OPTICK_GPU_EVENT("DeferredRenderingPass"); 
	auto frameBuffer = renderViewPort.GetCamera().GetFrameBuffer();
	const auto &alloc0 = GPUInstance.m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
	commandList->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView((int)eRootBindings::frameBuffer,
																			 alloc0.GpuAddress());

	GBuffer::Render(*this, commandList, scene);
	
}
void GraphicsEngine::EnvironmentLightPass(std::shared_ptr<CommandList> commandList)
{
	OPTICK_GPU_EVENT("EnvironmentLightPass");
	auto &environmentLight = m_Cache->GetState(PSOCache::ePipelineStateID::DeferredLighting);
	constexpr uint32_t bufferCount = 7;
	const auto &gBufferTextures = m_Cache->GetState(PSOCache::ePipelineStateID::GBuffer)->RenderTargets();

	// TODO READBACK BUFFER
	/*
	if (true)
	{
		const auto& resourceRef = gBufferTextures[5].GetResource();
		const size_t dataSize = gBufferTextures[5].GetWidth() * gBufferTextures[5].GetHeight() * sizeof(float) * 2;
		void* mapData = BufferForPicking;
		D3D12_RANGE readRange = { 0,dataSize};
		HRESULT hr = resourceRef->Map(0,&readRange,&mapData);

		if (SUCCEEDED(hr))
		{
			memcpy(BufferForPicking,mapData,dataSize);
			resourceRef->Unmap(0,nullptr);
		}
		WantPickingData = false;
	}*/

	{
		GPUInstance.ClearRTV(*commandList.get(), environmentLight->RenderTargets(),
							 environmentLight->GetNumberOfTargets());
		commandList->SetRenderTargets(environmentLight->GetNumberOfTargets(), environmentLight->RenderTargets(),
									  nullptr);

		const auto &pipelineState = environmentLight->GetPipelineState();
		commandList->GetGraphicsCommandList()->SetPipelineState(pipelineState.Get());
		commandList->TrackResource(pipelineState);

		for (unsigned i = 0; i < bufferCount; i++)
		{
			gBufferTextures[i].SetView(ViewType::SRV);
			commandList->TransitionBarrier(gBufferTextures[i], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->TrackResource(gBufferTextures[i]);
		}
		commandList->FlushResourceBarriers();
		commandList->SetDescriptorTable((int)eRootBindings::GbufferPasses, gBufferTextures);

		commandList->GetGraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, nullptr);
		commandList->GetGraphicsCommandList()->IASetIndexBuffer(nullptr);
		commandList->GetGraphicsCommandList()->DrawInstanced(6, 1, 0, 0);
	}
}
void GraphicsEngine::ToneMapperPass(std::shared_ptr<CommandList> commandList, Texture *target)
{
	OPTICK_GPU_EVENT("ToneMapperPass");
	const auto &toneMapper = m_Cache->GetState(PSOCache::ePipelineStateID::ToneMap);

	const auto &pipelineState = toneMapper->GetPipelineState().Get();
	commandList->GetGraphicsCommandList()->SetPipelineState(pipelineState);
	commandList->TrackResource(pipelineState);

	auto *renderTargets = m_Cache->GetState(PSOCache::ePipelineStateID::DeferredLighting)->RenderTargets();
	commandList->SetDescriptorTable((int)eRootBindings::TargetTexture, renderTargets);
	commandList->FlushResourceBarriers();

	GPUInstance.ClearRTV(*commandList.get(), toneMapper->RenderTargets(), toneMapper->GetNumberOfTargets());
	commandList->SetRenderTargets(1, target, nullptr);
	commandList->TrackResource(*target);

	commandList->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, nullptr);
	commandList->GetGraphicsCommandList()->IASetIndexBuffer(nullptr);
	commandList->GetGraphicsCommandList()->DrawInstanced(6, 1, 0, 0);
}

void GraphicsEngine::ImGuiPass()
{
	const auto commandQueue = GPUInstance.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	const auto commandList = commandQueue->GetCommandList();

	OPTICK_GPU_CONTEXT(commandList->GetGraphicsCommandList().Get());
	OPTICK_GPU_EVENT("ImGui");

	commandList->SetRenderTargets(1, &GPUInstance.GetCurrentBackBuffer(), nullptr);

	ImGui::Render();
	ImGuiIO &io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	ID3D12DescriptorHeap *heaps[] = {
		GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_CBV_SRV_UAV)]->Heap(),
		GPUInstance.m_ResourceDescriptors[static_cast<int>(eHeapTypes::HEAP_TYPE_SAMPLER)]->Heap()};
	commandList->GetGraphicsCommandList()->SetDescriptorHeaps((UINT)std::size(heaps), heaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList->GetGraphicsCommandList().Get());
	commandQueue->ExecuteCommandList(commandList);
}
