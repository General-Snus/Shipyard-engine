#pragma once
#include <DirectX/directx/d3d12.h>

class PSO;

class PSOCache
{
public:
	enum class ePipelineStateID : short
	{
		Default,
		GBuffer,
		Ambient,
		DeferredLighting,
		ToneMap,
		SSAO
	};

	void InitAllStates();

	std::unordered_map<ePipelineStateID,std::unique_ptr<PSO>> pso_map;
};


class PSO
{
public:
	PSO();
	void Init();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
};

