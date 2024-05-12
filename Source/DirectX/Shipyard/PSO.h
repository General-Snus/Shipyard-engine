#pragma once
#include <DirectX/directx/d3d12.h> 
#include <DirectX/Shipyard/Gpu_fwd.h>
#include <DirectX/Shipyard/Texture.h>
#include "Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h"

class cMeshRenderer;
class ShipyardShader;
class PSO;
using namespace Microsoft::WRL;

namespace GenerateMips
{
	enum
	{
		GenerateMipsCB,
		SrcMip,
		OutMip,
		NumRootParameters
	};
}

class PSOCache
{
public:
	enum class ePipelineStateID : short
	{
		Default,
		ShadowMapper,
		GBuffer,
		Ambient,
		DeferredLighting,
		ToneMap,
		SSAO,
		GenerateMips
	};

	static void InitRootSignature();
	static void InitAllStates();
	static std::unique_ptr<PSO>& GetState(ePipelineStateID id);
	static inline std::shared_ptr<GPURootSignature> m_RootSignature;
	static inline std::shared_ptr<GPURootSignature> m_MipRootSignature;
private:
	static void InitDefaultSignature();
	static void InitMipmapSignature();

	static inline std::unordered_map<ePipelineStateID,std::unique_ptr<PSO>> pso_map;
};


class PSO
{
public:
	static PSO CreatePSO(
		std::filesystem::path vertexShader,
		std::filesystem::path pixelShader,
		unsigned renderTargetAmount,
		DXGI_FORMAT renderTargetFormat,
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN
	);
	PSO() = default;
	virtual void Init(const ComPtr<ID3D12Device2>& dev);
	virtual Texture* GetRenderTargets() { return nullptr; };
	virtual uint16_t GetRenderTargetAmounts() { return 0; };
	virtual ComPtr<ID3D12PipelineState> GetPipelineState() const;
protected:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	ComPtr<ID3D12PipelineState> m_PipelineState;
	std::shared_ptr<ShipyardShader> vs;
	std::shared_ptr<ShipyardShader> ps;
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

	static inline constexpr uint16_t numRenderTargets = 7;
	Texture renderTargets[numRenderTargets];
};

class ShadowMapperPSO : public PSO
{
public:
	ShadowMapperPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;
	static void WriteShadows(std::shared_ptr<CommandList>& commandList);
};

class EnvironmentLightPSO : public PSO
{
public:
	EnvironmentLightPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;
	Texture* GetRenderTargets() override;
	uint16_t GetRenderTargetAmounts() override { return 1; };

	static LightBuffer CreateLightBuffer();
private:
	Texture renderTarget;
};

class TonemapPSO : public PSO
{
public:
	TonemapPSO() = default;
	void Init(const ComPtr<ID3D12Device2>& dev) override;
	Texture* GetRenderTargets() override;
	uint16_t GetRenderTargetAmounts() override { return 1; };
private:
	Texture renderTarget;
};

