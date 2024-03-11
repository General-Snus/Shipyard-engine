#pragma once
#include <DirectX/directx/d3d12.h>

class PSO;
using namespace Microsoft::WRL;

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

	static void InitAllStates();
	static std::unique_ptr<PSO>& GetState(ePipelineStateID id);
private:
	static inline std::unordered_map<ePipelineStateID,std::unique_ptr<PSO>> pso_map;
};


class PSO
{
public:
	PSO();
	void Init();
protected:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
};

class GbufferPSO : public PSO
{
public:
	GbufferPSO();
	void Init();
private:
	Texture renderTargets[8];
};

