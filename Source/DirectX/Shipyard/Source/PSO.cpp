#include "DirectXHeader.pch.h"

#include <dxgiformat.h>
#include "../GPU.h"
#include "../PSO.h" 
#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h" 

PSO::PSO()
{

}

void PSO::Init()
{
	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso",ps);

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

	stream.pRootSignature = GPU::m_RootSignature.GetRootSignature().Get();
	stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_pipelineState)));
}


void GbufferPSO::Init()
{
	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/GBufferPS.cso",ps);

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

	stream.pRootSignature = GPU::m_RootSignature.GetRootSignature().Get();
	stream.InputLayout = { Vertex::InputLayoutDefinition , Vertex::InputLayoutDefinitionSize };
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	stream.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBlob());
	stream.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBlob());
	stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	stream.RTVFormats = rtvFormats;

	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_pipelineState)));
}

void PSOCache::InitAllStates()
{
	std::unique_ptr<PSO> pso = std::make_unique<PSO>();
	pso->Init();
	pso_map[ePipelineStateID::Default] = std::move(pso); 

	std::unique_ptr<GbufferPSO> gbuffer = std::make_unique<GbufferPSO>();
	gbuffer->Init();
	pso_map[ePipelineStateID::GBuffer] = std::move(gbuffer);

}

std::unique_ptr<PSO>& PSOCache::GetState(ePipelineStateID id)
{
	return pso_map[id];
}
