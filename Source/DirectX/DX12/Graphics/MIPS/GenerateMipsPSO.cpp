#include <DirectXHeader.pch.h>

#include "Graphics/MIPS/GenerateMipsPSO.h"  

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Graphics/GPU.h"

//void GenerateMipsPSO::Init(const ComPtr<ID3D12Device2>& dev)
//{
//	auto device = dev;
//
//	struct PipelineStateStream
//	{
//		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
//		CD3DX12_PIPELINE_STATE_STREAM_CS CS;
//	} stream;
//
//
//	std::shared_ptr<ShipyardShader> CS;
//	EngineResources.ForceLoadAsset<ShipyardShader>("Shaders/GenerateMips_CS",CS);
//
//	stream.pRootSignature = PSOCache::m_MipRootSignature->GetRootSignature().Get();
//	stream.CS = CD3DX12_SHADER_BYTECODE(ps->GetBlob());
//
//	D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
//		sizeof(PipelineStateStream), &stream
//	};
//
//	Helpers::ThrowIfFailed(device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&m_PipelineState)));
//	m_DefaultUAV.CreateView(4);
//
//	for (UINT i = 0; i < 4; ++i)
//	{
//		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
//		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
//		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//		uavDesc.Texture2D.MipSlice = i;
//		uavDesc.Texture2D.PlaneSlice = 0;
//
//		device->CreateUnorderedAccessView(
//			nullptr,nullptr,&uavDesc,
//			m_DefaultUAV.GetHandle(i)
//		);
//	}
//}
