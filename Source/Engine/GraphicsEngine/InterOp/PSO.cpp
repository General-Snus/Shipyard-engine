#include "GraphicsEngine.pch.h"
#include <DirectX/directx/d3dx12_core.h> 
#include "GPU.h"
#include "PSO.h" 

#include "Shaders/Include/Default_PS.h"
#include "Shaders/Include/Default_VS.h"

PSO::PSO()
{

}

void PSO::Init()
{



#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	GPU::CreatePixelShader(pixelShader,BuiltIn_Default_PS_ByteCode,sizeof(BuiltIn_Default_PS_ByteCode),compileFlags);
	GPU::CreatePixelShader(vertexShader,BuiltIn_Default_VS_ByteCode,sizeof(BuiltIn_Default_VS_ByteCode),compileFlags);


	psoDesc.InputLayout = { Vertex::InputLayoutDefinition, _countof(Vertex::InputLayoutDefinition) };
	psoDesc.pRootSignature = GPU::m_RootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(BuiltIn_Default_VS_ByteCode,sizeof(BuiltIn_Default_VS_ByteCode)); // add shader cache later or never u lazy sob
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(BuiltIn_Default_PS_ByteCode,sizeof(BuiltIn_Default_PS_ByteCode));
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	if (FAILED(GPU::m_Device->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&m_pipelineState))))
	{
		Logger::Err("Failed to create pipeline state");
	}

	GPU::m_Device->CreateCommandList(0,D3D12_COMMAND_LIST_TYPE_DIRECT,GPU::m_Allocator.Get(),m_pipelineState.Get(),IID_PPV_ARGS(&GPU::m_CommandList));

	GPU::m_CommandList->Close();
}

void PSOCache::InitAllStates()
{
	std::unique_ptr<PSO> pso = std::make_unique<PSO>();
	pso->Init();
	pso_map[ePipelineStateID::Default] = std::move(pso);

}
