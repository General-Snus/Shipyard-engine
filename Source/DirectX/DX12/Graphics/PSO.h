#pragma once
#include <source_location>
#include <span>
#include <DirectX/DX12/Graphics/Gpu_fwd.h>
#include <DirectX/DX12/Graphics/Helpers.h>
#include <DirectX/DX12/Graphics/Resources/Texture.h>
#include <Engine/AssetManager/ComponentSystem/GameObjectManager.h>
#include <Tools/Utilities/Error.hpp>
#include "Engine/GraphicsEngine/Rendering/Buffers/LightBuffer.h"

class MeshRenderer;
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
} // namespace GenerateMips

class PSOCache
{
public:
	friend class PSO;

	enum class ePipelineStateID : short
	{
		Default,
		ShadowMapper,
		GBuffer,
		Ambient,
		DeferredLighting,
		ToneMap,
		SSAO,
		DebugDraw,
		GenerateMips
	};

	void                              InitRootSignature();
	void                              InitAllStates();
	std::unique_ptr<PSO>&             GetState(ePipelineStateID id);
	const std::unique_ptr<PSO>&       GetState(ePipelineStateID id) const;
	std::shared_ptr<GPURootSignature> m_RootSignature;
	std::shared_ptr<GPURootSignature> m_MipRootSignature;

	std::unique_ptr<PSO> CreatePSO(
		const std::filesystem::path& vertexShader, const std::filesystem::path& pixelShader,
		std::span<const DXGI_FORMAT> renderTargetFormat,
		DXGI_FORMAT                  depthStencilFormat = DXGI_FORMAT_UNKNOWN,
		const CD3DX12_BLEND_DESC&    desc = CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
		std::wstring_view            name = std::format(
			L"LowLifetime PSO from {}",
			Helpers::string_cast<std::wstring>(std::string(std::source_location::current().function_name()))),
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

private:
	void InitDefaultSignature();
	void InitMipmapSignature();

	std::unordered_map<ePipelineStateID, std::unique_ptr<PSO>> pso_map;
};

class PSO
{
public:
	friend class PSOCache;

	Texture* RenderTargets()
	{
		return m_renderTargets.data();
	};

	uint32_t GetNumberOfTargets() const
	{
		return m_numRenderTargets;
	};
	const Ref<ID3D12PipelineState>& GetPipelineState() const;

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDescription = {};
	Ref<ID3D12PipelineState>        m_PipelineState;

	unsigned int                    m_numRenderTargets = 0;
	std::vector<Texture>            m_renderTargets;
	std::shared_ptr<ShipyardShader> m_vs;
	std::shared_ptr<ShipyardShader> m_ps;
	Ref<ID3D12Device2>           m_Device;
};
