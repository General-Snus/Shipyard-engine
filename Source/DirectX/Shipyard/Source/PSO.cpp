#include "DirectXHeader.pch.h"

#include <DirectX/Shipyard/RootSignature.h>
#include <dxgiformat.h>
#include <Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h>
#include <Engine/GraphicsEngine/Shaders/Registers.h>
#include <strsafe.h>
#include "../GPU.h"
#include "../PSO.h" 

#include <Tools/Optick/include/optick.h>

#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h"
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h" 

#include "Engine/AssetManager/ComponentSystem/Components/LightComponent.h"
#include "Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h"
#include "Engine/AssetManager/ComponentSystem/Components/Transform.h"
#include "Engine/AssetManager/ComponentSystem/GameObjectManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/MaterialAsset.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/FrameBuffer.h"
#include "Engine/GraphicsEngine/Rendering/Buffers/ObjectBuffer.h"
#include "Shipyard/MIPS/GenerateMipsPSO.h"

void PSOCache::InitAllStates()
{

	InitRootSignature();

	auto pso = std::make_unique<PSO>();
	pso->Init(GPU::m_Device);
	pso_map[ePipelineStateID::Default] = std::move(pso);


	auto shadowMapper = std::make_unique<ShadowMapperPSO>();
	shadowMapper->Init(GPU::m_Device);
	pso_map[ePipelineStateID::ShadowMapper] = std::move(shadowMapper);

	auto gbuffer = std::make_unique<GbufferPSO>();
	gbuffer->Init(GPU::m_Device);
	pso_map[ePipelineStateID::GBuffer] = std::move(gbuffer);

	auto env = std::make_unique<EnvironmentLightPSO>();
	env->Init(GPU::m_Device);
	pso_map[ePipelineStateID::DeferredLighting] = std::move(env);

	auto tonemapPSO = std::make_unique<TonemapPSO>();
	tonemapPSO->Init(GPU::m_Device);
	pso_map[ePipelineStateID::ToneMap] = std::move(tonemapPSO);

}

std::unique_ptr<PSO>& PSOCache::GetState(ePipelineStateID id)
{
	OPTICK_EVENT();
	return pso_map[id];
}

void PSOCache::InitDefaultSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0,nullptr,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,&signature,&error));
	Helpers::ThrowIfFailed(GPU::m_Device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature->GetRootSignature())
	));


	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[NumRootParameters];
	rootParameters[eRootBindings::frameBuffer].InitAsConstantBufferView(REG_FrameBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::objectBuffer].InitAsConstantBufferView(REG_ObjectBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::materialBuffer].InitAsConstantBufferView(REG_DefaultMaterialBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::lightBuffer].InitAsConstantBufferView(REG_LightBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);

	const auto descriptorRange = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,2048,REG_colorMap,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::Textures].InitAsDescriptorTable(1,&descriptorRange,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto GbufferPasses = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,8,REG_colorMap,1,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::GbufferPasses].InitAsDescriptorTable(1,&GbufferPasses,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto TargetTexture = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,5,REG_Target0,2,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::TargetTexture].InitAsDescriptorTable(1,&TargetTexture,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto PermanentTextures = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,5,REG_enviromentCube,3,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::PermanentTextures].InitAsDescriptorTable(1,&PermanentTextures,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto meshBuffer = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,4048,REG_colorMap,4,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::MeshBuffer].InitAsDescriptorTable(1,&meshBuffer,D3D12_SHADER_VISIBILITY_VERTEX);

	const CD3DX12_STATIC_SAMPLER_DESC samplers[]
	{
		CD3DX12_STATIC_SAMPLER_DESC(REG_DefaultSampler,D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR),

		CD3DX12_STATIC_SAMPLER_DESC(REG_BRDFSampler,
									D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									0,1,D3D12_COMPARISON_FUNC_NEVER,D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,0,0),

		CD3DX12_STATIC_SAMPLER_DESC(REG_shadowCmpSampler,
									D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									0,1,D3D12_COMPARISON_FUNC_GREATER_EQUAL,D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,-D3D12_FLOAT32_MAX,D3D12_FLOAT32_MAX)
	};

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(NumRootParameters,rootParameters,(unsigned)std::size(samplers),samplers,rootSignatureFlags);

	m_RootSignature->SetRootSignatureDesc(rootSignatureDescription.Desc_1_1,GPU::m_FeatureData);
}

void PSOCache::InitMipmapSignature()
{
	auto device = GPU::m_Device;
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,&featureData,sizeof(featureData))))
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0,nullptr,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,&signature,&error));
		Helpers::ThrowIfFailed(GPU::m_Device->CreateRootSignature(
			0,
			signature->GetBufferPointer(),
			signature->GetBufferSize(),
			IID_PPV_ARGS(&m_MipRootSignature->GetRootSignature())
		));
	}


	CD3DX12_DESCRIPTOR_RANGE1 srcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,1,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	CD3DX12_DESCRIPTOR_RANGE1 outMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,4,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

	CD3DX12_ROOT_PARAMETER1 rootParameters[GenerateMips::NumRootParameters];
	rootParameters[GenerateMips::GenerateMipsCB].InitAsConstants(sizeof(GenerateMipsCB) / 4,0);
	rootParameters[GenerateMips::SrcMip].InitAsDescriptorTable(1,&srcMip);
	rootParameters[GenerateMips::OutMip].InitAsDescriptorTable(1,&outMip);

	CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(GenerateMips::NumRootParameters,rootParameters,1,&linearClampSampler);

	m_MipRootSignature->SetRootSignatureDesc(rootSignatureDesc.Desc_1_1,featureData.HighestVersion);
}

void PSOCache::InitRootSignature()
{
	m_RootSignature = std::make_shared<GPURootSignature>();
	m_MipRootSignature = std::make_shared<GPURootSignature>();

	InitDefaultSignature();
	InitMipmapSignature();

}

PSO PSO::CreatePSO(std::filesystem::path vertexShader,std::filesystem::path pixelShader,unsigned renderTargetAmount,
	DXGI_FORMAT renderTargetFormat,DXGI_FORMAT depthStencilFormat)
{
	PSO pso;
	pso.m_Device = GPU::m_Device;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>(vertexShader.string(),pso.vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>(pixelShader.string(),pso.ps);


	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		//CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
	}  stream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	for (size_t i = 0; i < renderTargetAmount; i++)
	{
		rtvFormats.RTFormats[i] = { renderTargetFormat };
	}
	rtvFormats.NumRenderTargets = renderTargetAmount;

	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	//stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(pso.vs->GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(pso.ps->GetBlob());
	stream.DSVFormat = depthStencilFormat;
	stream.RTVFormats = rtvFormats;

	auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	stream.DepthStencilState = depthStencil;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&pso.m_PipelineState)));
	pso.m_PipelineState->SetName(L"LowLifetimePSO");

	return pso;
}

void PSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso",ps);

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		//CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
	}  stream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.RTFormats[0] = { GPU::m_BackBuffer->GetResource()->GetDesc().Format };
	rtvFormats.NumRenderTargets = 1;

	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	//stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	stream.DepthStencilState = depthStencil;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
	m_PipelineState->SetName(L"Default pipeline state");
}
ComPtr<ID3D12PipelineState> PSO::GetPipelineState() const
{
	return m_PipelineState;
}


void GbufferPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/GBufferPS.cso",ps);

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		//CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
	}  stream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};

	CD3DX12_BLEND_DESC desc = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT());
	desc.IndependentBlendEnable = true;


	renderTargets[0].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"ColorLayer",
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	renderTargets[1].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"NormalLayer",
		DXGI_FORMAT_R16G16B16A16_SNORM,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);

	renderTargets[2].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"MaterialLayer",
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);

	renderTargets[3].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"EmmissionLayer",
		DXGI_FORMAT_R8G8B8A8_UNORM,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);

	renderTargets[4].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"VertexNormalLayer",
		DXGI_FORMAT_R16G16B16A16_SNORM,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);

	renderTargets[5].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"WorldPositionLayer",
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);
	renderTargets[6].AllocateTexture(
		{ GPU::m_Width,GPU::m_Height },
		"ZDepthLayer",
		DXGI_FORMAT_R32_FLOAT,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
	);

	rtvFormats.NumRenderTargets = numRenderTargets;
	for (int i = 0; i < numRenderTargets; i++)
	{
		rtvFormats.RTFormats[i] = renderTargets[i].GetResource()->GetDesc().Format;
	}

	stream.BlendDesc = desc;

	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	//stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	stream.DepthStencilState = depthStencil;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
	m_PipelineState->SetName(L"GbufferPSO");
}
Texture* GbufferPSO::GetRenderTargets()
{
	return renderTargets;
}


void ShadowMapperPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		//CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
	}  stream;


	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	//stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	stream.DepthStencilState = depthStencil;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
	m_PipelineState->SetName(L"ShadowMapperPSO");
}

void ShadowMapperPSO::WriteShadows(std::shared_ptr<CommandList>& commandList)
{
	OPTICK_EVENT();


	static auto& objectsToRender = GameObjectManager::Get().GetAllComponents<cMeshRenderer>();


	std::shared_ptr<Texture> shadowMap = nullptr;
	const auto graphicCommandList = commandList->GetGraphicsCommandList();

	const auto& shadowMapper = PSOCache::GetState(PSOCache::ePipelineStateID::ShadowMapper);

	const auto& pipelineState = shadowMapper->GetPipelineState().Get();
	graphicCommandList->SetPipelineState(pipelineState);
	commandList->TrackResource(pipelineState);
	MaterialBuffer materialBuffer;

	for (auto& light : GameObjectManager::Get().GetAllComponents<cLight>())
	{
		if (!light.IsActive())
		{
			continue;
		}

		if (light.GetIsShadowCaster())
		{
			if (light.GetType() == eLightType::Directional && !light.GetIsRendered())
			{
				shadowMap = light.GetShadowMap(0);

				graphicCommandList->RSSetViewports(1,&shadowMap->GetViewPort());
				graphicCommandList->RSSetScissorRects(1,&shadowMap->GetRect());

				commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_WRITE);
				GPU::ClearDepth(*commandList,shadowMap->GetHandle(ViewType::DSV).cpuPtr);
				commandList->SetRenderTargets(0,nullptr,shadowMap.get());

				auto frameBuffer = light.GetShadowMapFrameBuffer(0);
				const auto& alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
				graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::frameBuffer,alloc0.GpuAddress());

				for (const auto& object : objectsToRender)
				{
					if (!object.IsActive())
					{
						continue;
					}

					const auto& transform = object.GetComponent<Transform>();
					ObjectBuffer objectBuffer;
					objectBuffer.myTransform = transform.GetRawTransform();

					const auto& alloc1 = GPU::m_GraphicsMemory->AllocateConstant<ObjectBuffer>(objectBuffer);
					graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::objectBuffer,alloc1.GpuAddress());

					for (auto& element : object.GetElements())
					{
						GPU::ConfigureInputAssembler(*commandList,D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,element.VertexBuffer,element.IndexResource);

						materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
						materialBuffer.vertexOffset = 0; //vertex offset is part of drawcall, if i ever use this i need to set it here 

						const auto& alloc2 = GPU::m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
						graphicCommandList->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,alloc2.GpuAddress());

						OPTICK_GPU_EVENT("DirectionalShadowDraw");
						graphicCommandList->DrawIndexedInstanced(element.IndexResource.GetIndexCount(),1,0,0,0);
					}
				}
				shadowMap->SetView(ViewType::SRV);
				commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				commandList->TrackResource(*shadowMap);
			}

			if (light.GetType() == eLightType::Spot && !light.GetIsRendered())
			{
				shadowMap = light.GetShadowMap(0);

				graphicCommandList->RSSetViewports(1,&shadowMap->GetViewPort());
				graphicCommandList->RSSetScissorRects(1,&shadowMap->GetRect());

				commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_WRITE);
				GPU::ClearDepth(*commandList,shadowMap->GetHandle(ViewType::DSV).cpuPtr);
				commandList->SetRenderTargets(0,nullptr,shadowMap.get());

				auto frameBuffer = light.GetShadowMapFrameBuffer(0);
				const auto& alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
				graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::frameBuffer,alloc0.GpuAddress());

				for (const auto& object : objectsToRender)
				{
					if (!object.IsActive())
					{
						continue;
					}

					const auto& transform = object.GetComponent<Transform>(); ObjectBuffer objectBuffer;
					objectBuffer.myTransform = transform.GetRawTransform();

					const auto& alloc1 = GPU::m_GraphicsMemory->AllocateConstant<ObjectBuffer>(objectBuffer);
					graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::objectBuffer,alloc1.GpuAddress());

					for (auto& element : object.GetElements())
					{
						GPU::ConfigureInputAssembler(*commandList,D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,element.VertexBuffer,element.IndexResource);

						materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
						materialBuffer.vertexOffset = 0; //vertex offset is part of drawcall, if i ever use this i need to set it here 

						const auto& alloc2 = GPU::m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
						graphicCommandList->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,alloc2.GpuAddress());

						OPTICK_GPU_EVENT("SpotlightShadowDraw");
						graphicCommandList->DrawIndexedInstanced(element.IndexResource.GetIndexCount(),1,0,0,0);
					}
				}
				shadowMap->SetView(ViewType::SRV);
				commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				commandList->TrackResource(*shadowMap);
			}

			if (light.GetType() == eLightType::Point && !light.GetIsRendered())
			{
				for (int j = 0; j < 6; j++)
				{
					shadowMap = light.GetShadowMap(j);

					graphicCommandList->RSSetViewports(1,&shadowMap->GetViewPort());
					graphicCommandList->RSSetScissorRects(1,&shadowMap->GetRect());

					commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_WRITE);
					GPU::ClearDepth(*commandList,shadowMap->GetHandle(ViewType::DSV).cpuPtr);
					commandList->SetRenderTargets(0,nullptr,shadowMap.get());

					auto frameBuffer = light.GetShadowMapFrameBuffer(j);
					const auto& alloc0 = GPU::m_GraphicsMemory->AllocateConstant<FrameBuffer>(frameBuffer);
					graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::frameBuffer,alloc0.GpuAddress());

					for (const auto& object : objectsToRender)
					{
						if (!object.IsActive())
						{
							continue;
						}

						const auto& transform = object.GetComponent<Transform>();
						ObjectBuffer objectBuffer;
						objectBuffer.myTransform = transform.GetRawTransform();

						const auto& alloc1 = GPU::m_GraphicsMemory->AllocateConstant<ObjectBuffer>(objectBuffer);
						graphicCommandList->SetGraphicsRootConstantBufferView(eRootBindings::objectBuffer,alloc1.GpuAddress());

						for (auto& element : object.GetElements())
						{
							GPU::ConfigureInputAssembler(*commandList,D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,element.VertexBuffer,element.IndexResource);

							materialBuffer.vertexBufferIndex = element.VertexBuffer.GetHandle(ViewType::SRV).heapOffset;
							materialBuffer.vertexOffset = 0; //vertex offset is part of drawcall, if i ever use this i need to set it here 

							const auto& alloc2 = GPU::m_GraphicsMemory->AllocateConstant<MaterialBuffer>(materialBuffer);
							graphicCommandList->SetGraphicsRootConstantBufferView(REG_DefaultMaterialBuffer,alloc2.GpuAddress());

							OPTICK_GPU_EVENT("PointlightShadowDraw");
							graphicCommandList->DrawIndexedInstanced(element.IndexResource.GetIndexCount(),1,0,0,0);
						}
					}
					shadowMap->SetView(ViewType::SRV);
					commandList->TransitionBarrier(*shadowMap,D3D12_RESOURCE_STATE_DEPTH_READ | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					commandList->TrackResource(*shadowMap);
				}
			}
		}
		light.SetIsRendered(true);
	}
}

void EnvironmentLightPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/ScreenspaceQuad_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/EnvironmentLight_PS.cso",ps);

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		//CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	}  stream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};

	renderTarget.AllocateTexture({ Window::Width(),Window::Height() },"Target0");
	rtvFormats.RTFormats[0] = { renderTarget.GetResource()->GetDesc().Format };
	rtvFormats.NumRenderTargets = 1;



	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetBlob());
	stream.DSVFormat = DXGI_FORMAT_UNKNOWN;
	stream.RTVFormats = rtvFormats;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
	m_PipelineState->SetName(L"EnvironmentLightPSO");
}
Texture* EnvironmentLightPSO::GetRenderTargets()
{
	return &renderTarget;
}
LightBuffer EnvironmentLightPSO::CreateLightBuffer()
{
	OPTICK_EVENT();
	std::vector<DirectionalLight*> dirLight;
	std::vector<PointLight*> pointLight;
	std::vector< SpotLight* > spotLight;

	for (auto& i : GameObjectManager::Get().GetAllComponents<cLight>())
	{
		switch (i.GetType())
		{
		case eLightType::Directional:
		{
			auto* data = i.GetData<DirectionalLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}

			dirLight.push_back(data);
			break;
		}
		//REFACTOR
		case eLightType::Point:
		{
			auto* data = i.GetData<PointLight>().get();
			if (i.GetIsShadowCaster())
			{
				for (int j = 0; j < 6; j++)
				{
					data->shadowMapIndex[j] = i.GetShadowMap(j)->GetHandle(ViewType::SRV).heapOffset;
				}
			}
			pointLight.push_back(data);
			break;
		}
		case eLightType::Spot:
		{
			auto* data = i.GetData<SpotLight>().get();
			if (i.GetIsShadowCaster())
			{
				data->shadowMapIndex = i.GetShadowMap(0)->GetHandle(ViewType::SRV).heapOffset;
			}
			spotLight.push_back(data);
			break;
		}

		default:
			break;
		}
	}

	LightBuffer lightBuffer;

	for (int i = 0; i < dirLight.size(); ++i)
	{
		lightBuffer.directionalLight = *dirLight[i];
	}

	lightBuffer.pointLightAmount = std::min(static_cast<int>(pointLight.size()),8);
	for (int i = 0; i < lightBuffer.pointLightAmount; ++i)
	{
		lightBuffer.pointLight[i] = *pointLight[i];
	}


	lightBuffer.spotLightAmount = std::min(static_cast<int>(spotLight.size()),8);
	for (int i = 0; i < lightBuffer.spotLightAmount; ++i)
	{
		lightBuffer.spotLight[i] = *spotLight[i];
	}

	return lightBuffer;
}


void TonemapPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/ScreenspaceQuad_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/ToneMapping_PS.cso",ps);

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	}  stream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	renderTarget.AllocateTexture({ Window::Width(),Window::Height() },"Target1");
	rtvFormats.RTFormats[0] = { renderTarget.GetResource()->GetDesc().Format };
	rtvFormats.NumRenderTargets = 1;

	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetBlob());
	stream.DSVFormat = DXGI_FORMAT_UNKNOWN;
	stream.RTVFormats = rtvFormats;

	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
	m_PipelineState->SetName(L"TonemapPSO");
}
Texture* TonemapPSO::GetRenderTargets()
{
	return &renderTarget;
}