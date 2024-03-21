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
	PSO() = default;
	virtual void Init(const ComPtr<ID3D12Device2>& dev);
	virtual Texture* GetRenderTargets() { return nullptr; };
	virtual uint16_t GetRenderTargetAmounts() { return 0; };
	virtual ComPtr<ID3D12PipelineState> GetPipelineState() const;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const
	{
		return m_RootSignature.GetRootSignature();
	}
protected:
	virtual void InitRootSignature();

	GPURootSignature m_RootSignature;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;
	ComPtr<ID3D12Device2> m_Device;
};

class GbufferPSO : public PSO
{
public:
	GbufferPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;
	Texture* GetRenderTargets() override;	
	uint16_t GetRenderTargetAmounts() override { return numRenderTargets; };

private:
	  void InitRootSignature() override;

	static inline constexpr uint16_t numRenderTargets = 8;
	Texture renderTargets[numRenderTargets];
};

class EnviromentLightPSO : public PSO
{
public:
	EnviromentLightPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;

	void SetResources(Texture* texArray, int amount) override;
	Texture* GetRenderTargets() override;
	uint16_t GetRenderTargetAmounts() override { return 1; };

private:
	static inline constexpr uint16_t numRenderTargets = 8;
	Texture textureResources[numRenderTargets];
	void InitRootSignature() override;
};

class TonemapPSO : public PSO
{
public:
	TonemapPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;
	Texture* GetRenderTargets() override;
	uint16_t GetRenderTargetAmounts() override { return 1; };

private:
	void InitRootSignature() override; 
};

