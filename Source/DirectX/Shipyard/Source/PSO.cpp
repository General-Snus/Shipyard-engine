#include "DirectXHeader.pch.h"

#include <dxgiformat.h>
#include "../GPU.h"
#include "../PSO.h" 
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h" 
#include <strsafe.h>
#include <Engine/GraphicsEngine/Shaders/Registers.h>
#include <DirectX/Shipyard/RootSignature.h>

void PSOCache::InitAllStates()
{
	std::unique_ptr<PSO> pso = std::make_unique<PSO>();
	pso->Init(GPU::m_Device);
	pso_map[ePipelineStateID::Default] = std::move(pso);

	std::unique_ptr<GbufferPSO> gbuffer = std::make_unique<GbufferPSO>();
	gbuffer->Init(GPU::m_Device);
	pso_map[ePipelineStateID::GBuffer] = std::move(gbuffer);

	std::unique_ptr<TonemapPSO> tonemapPSO = std::make_unique<TonemapPSO>();
	tonemapPSO->Init(GPU::m_Device);
	pso_map[ePipelineStateID::ToneMap] = std::move(tonemapPSO);

}

std::unique_ptr<PSO>& PSOCache::GetState(ePipelineStateID id)
{
	return pso_map[id];
}

void PSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;
	InitRootSignature();

	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso", vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso", ps);

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
	rtvFormats.RTFormats[0] = { GPU::m_BackBuffer->GetResource()->GetDesc().Format };
	rtvFormats.NumRenderTargets = 1;

	stream.pRootSignature = m_RootSignature.GetRootSignature().Get();
	stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void PSO::InitRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Helpers::ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature.GetRootSignature())));


	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[NumRootParameters];
	rootParameters[eRootBindings::frameBuffer].InitAsConstantBufferView(REG_FrameBuffer, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::objectBuffer].InitAsConstantBufferView(REG_ObjectBuffer, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REG_colorMap);
	rootParameters[eRootBindings::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(REG_DefaultSampler, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	linearRepeatSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; // Im inverse depth for large value precicion

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

	m_RootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, GPU::m_FeatureData.HighestVersion);
}

ComPtr<ID3D12PipelineState> PSO::GetPipelineState() const
{
	return m_pipelineState;
}



void GbufferPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;
	InitRootSignature();


	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso", vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/GBufferPS.cso", ps);

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
	for (size_t i = 0; i < numRenderTargets; i++)
	{
		renderTargets[i].AllocateTexture(GPU::m_Width, GPU::m_Height, "GBufferRenderTexture" + std::to_string(i));
		rtvFormats.RTFormats[i] = { renderTargets[i].GetResource()->GetDesc().Format };
	}
	rtvFormats.NumRenderTargets = numRenderTargets;

	stream.pRootSignature = m_RootSignature.GetRootSignature().Get();
	stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc, IID_PPV_ARGS(&m_pipelineState)));
}

Texture* GbufferPSO::GetRenderTargets()
{ 
	return renderTargets;
}

void GbufferPSO::InitRootSignature()
{
	PSO::InitRootSignature();
}


void TonemapPSO::Init(const ComPtr<ID3D12Device2>& dev)
{
	m_Device = dev;
	InitRootSignature();
	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/ScreenspaceQuad_VS.cso", vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/ToneMapping_PS.cso", ps);

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
	rtvFormats.RTFormats[0] = { GPU::m_BackBuffer->GetResource()->GetDesc().Format };
	rtvFormats.NumRenderTargets = 1;

	stream.pRootSignature = m_RootSignature.GetRootSignature().Get();
	/*stream.InputLayout =
	{
		{ "SV_VertexID",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		1
	};*/
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc, IID_PPV_ARGS(&m_pipelineState)));
}

Texture* TonemapPSO::GetRenderTargets()
{
	return GPU::GetCurrentBackBuffer();
}

void TonemapPSO::InitRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	Helpers::ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature.GetRootSignature())));


	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(REG_DefaultSampler, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
	linearRepeatSampler.ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL; // Im inverse depth for large value precicion

	CD3DX12_ROOT_PARAMETER1 rootParameters[NumRootParameters];


	rootParameters[eRootBindings::frameBuffer].InitAsConstantBufferView(REG_FrameBuffer, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::objectBuffer].InitAsConstantBufferView(REG_ObjectBuffer, 0, D3D12_ROOT_DESCRIPTOR_FLAG_NONE, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_DESCRIPTOR_RANGE1 descriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, REG_Target01);
	rootParameters[eRootBindings::Textures].InitAsDescriptorTable(1, &descriptorRange, D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(NumRootParameters, rootParameters, 1, &linearRepeatSampler, rootSignatureFlags);

	m_RootSignature.SetRootSignatureDesc(rootSignatureDescription.Desc_1_1, GPU::m_FeatureData.HighestVersion);
}
