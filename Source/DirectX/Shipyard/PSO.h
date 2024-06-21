#pragma once
#include <span> 
#include <DirectX/Shipyard/Helpers.h>
#include <source_location> 
#include <DirectX/directx/d3d12.h> 
#include <DirectX/directx/d3dx12_core.h> 
#include <DirectX/Shipyard/Gpu_fwd.h>
#include <DirectX/Shipyard/Texture.h>
#include "Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h"
#include <Tools/Utilities/Error.hpp>
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>

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
	static std::unique_ptr<PSO> CreatePSO(
		const std::filesystem::path& vertexShader,
		const std::filesystem::path& pixelShader,
		const std::span<const DXGI_FORMAT> renderTargetFormat,
		DXGI_FORMAT depthStencilFormat = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN,
		const CD3DX12_BLEND_DESC& desc = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
		std::wstring_view name = std::format(
			L"LowLifetime PSO from {}",
			Helpers::string_cast<std::wstring> (std::string(std::source_location::current().function_name()))
		)
	);

	Texture* RenderTargets() { return m_renderTargets.data(); };
	uint32_t GetNumberOfTargets() const { return m_numRenderTargets; };
	ComPtr<ID3D12PipelineState> GetPipelineState() const;

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDescription = {};
	ComPtr<ID3D12PipelineState> m_PipelineState;

	unsigned int m_numRenderTargets = 0;
	std::vector<Texture> m_renderTargets;
	std::shared_ptr<ShipyardShader> m_vs;
	std::shared_ptr<ShipyardShader> m_ps;
	ComPtr<ID3D12Device2> m_Device;
};

