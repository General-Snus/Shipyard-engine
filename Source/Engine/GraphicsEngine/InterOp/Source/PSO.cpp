#include "GraphicsEngine.pch.h"
#include <d3dcompiler.h>
#include <DirectX/directx/d3dx12_core.h> 
#include "../GPU.h"
#include "../PSO.h"  


#include "Engine/AssetManager/AssetManager.h"
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h"
#include "Shaders/Include/Default_PS.h"
#include "Shaders/Include/Default_VS.h"

PSO::PSO()
{

}

void PSO::Init()
{
	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;

	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_VS.cso",vs);
	AssetManager::Get().ForceLoadAsset<ShipyardShader>("Shaders/Default_PS.cso",ps);


	//GPU::CreatePixelShader(pixelShader,BuiltIn_Default_PS_ByteCode,sizeof(BuiltIn_Default_PS_ByteCode),compileFlags);
	//GPU::CreateVertexShader(vertexShader,BuiltIn_Default_VS_ByteCode,sizeof(BuiltIn_Default_VS_ByteCode),compileFlags);

	D3D12_INPUT_LAYOUT_DESC inputLayout = {};
	inputLayout.pInputElementDescs = Vertex::InputLayoutDefinition.data();
	inputLayout.NumElements = (UINT)Vertex::InputLayoutDefinition.size();

	psoDesc.InputLayout = inputLayout; //{ Vertex::InputLayoutDefinition,_countof(Vertex::InputLayoutDefinition) };
	psoDesc.pRootSignature = GPU::m_RootSignature.GetSignature();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetShader().GetBufferPtr(),vs->GetShader().GetBlobSize()); // add shader cache later or never u lazy sob
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetShader().GetBufferPtr(),ps->GetShader().GetBlobSize());
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	Helpers::ThrowIfFailed(GPU::m_Device->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf())));
	/*GPU::m_Device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,GPU::m_Allocator.Get(),m_pipelineState.Get(),IID_PPV_ARGS(&GPU::m_CommandList));

	GPU::m_CommandList->Close();*/
}

void PSOCache::InitAllStates()
{
	std::unique_ptr<PSO> pso = std::make_unique<PSO>();
	pso->Init();
	pso_map[ePipelineStateID::Default] = std::move(pso);

}
