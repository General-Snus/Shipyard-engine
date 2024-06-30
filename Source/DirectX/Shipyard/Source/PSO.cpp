#include "DirectX/DirectXHeader.pch.h"

#include <DirectX/Shipyard/RootSignature.h>
#include <Engine/GraphicsEngine/Shaders/Registers.h>
#include "../GPU.h"
#include "../PSO.h"  
#include "Engine/AssetManager/AssetManager.h" 
#include "Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h" 
	

void PSOCache::InitAllStates()
{

	InitRootSignature(); 
	{
		constexpr std::array rtvFormats = {
			DXGI_FORMAT_R8G8B8A8_UNORM,		//Albedo
			DXGI_FORMAT_R16G16B16A16_SNORM,	//Normal
			DXGI_FORMAT_R8G8B8A8_UNORM,		//Material
			DXGI_FORMAT_R8G8B8A8_UNORM,		//Effect(R)
			DXGI_FORMAT_R16G16B16A16_SNORM,	//Vertex Normal
			DXGI_FORMAT_R32G32B32A32_FLOAT, //World position
			DXGI_FORMAT_R32G32_FLOAT		//Depth(R)
		};
		auto pso = PSO::CreatePSO(
			"Shaders/Default_VS.cso",
			"Shaders/GBufferPS.cso",
			rtvFormats,DXGI_FORMAT_D32_FLOAT,
			CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
			L"G Buffer"
		);

		pso_map[ePipelineStateID::GBuffer] = std::move(pso);
	}

	{

		auto pso = PSO::CreatePSO(
			"Shaders/Default_VS.cso",
			"",
			{},DXGI_FORMAT_D32_FLOAT,
			CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
			L"ShadowMapping"
		);

		pso_map[ePipelineStateID::ShadowMapper] = std::move(pso);
	}
	{
		constexpr std::array rtvFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
		auto pso = PSO::CreatePSO(
			"Shaders/ScreenspaceQuad_VS.cso",
			"Shaders/EnvironmentLight_PS.cso",
			rtvFormats,DXGI_FORMAT_UNKNOWN,
			CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
			L"DeferredLighting"
		);

		pso_map[ePipelineStateID::DeferredLighting] = std::move(pso);
	}

	{
		constexpr std::array rtvFormats = { DXGI_FORMAT_R8G8B8A8_UNORM };
		auto pso = PSO::CreatePSO(
			"Shaders/ScreenspaceQuad_VS.cso",
			"Shaders/ToneMapping_PS.cso",
			rtvFormats,DXGI_FORMAT_UNKNOWN,
			CD3DX12_BLEND_DESC(CD3DX12_DEFAULT()),
			L"ToneMap"
		);
		pso_map[ePipelineStateID::ToneMap] = std::move(pso);
	}
}

std::unique_ptr<PSO>& PSOCache::GetState(ePipelineStateID id)
{
	OPTICK_EVENT();
	return pso_map[id];
}

void PSOCache::InitDefaultSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0,nullptr,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,&signature,&error));
	Helpers::ThrowIfFailed(GPU::m_Device->CreateRootSignature(
		0,
		signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&m_RootSignature->GetRootSignature())
	));


	const D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_ROOT_PARAMETER1 rootParameters[NumRootParameters];
	rootParameters[eRootBindings::frameBuffer].InitAsConstantBufferView(REG_FrameBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::objectBuffer].InitAsConstantBufferView(REG_ObjectBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::materialBuffer].InitAsConstantBufferView(REG_DefaultMaterialBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[eRootBindings::lightBuffer].InitAsConstantBufferView(REG_LightBuffer,0,D3D12_ROOT_DESCRIPTOR_FLAG_NONE,D3D12_SHADER_VISIBILITY_ALL);

	const auto descriptorRange = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,2048,REG_colorMap,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::Textures].InitAsDescriptorTable(1,&descriptorRange,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto GbufferPasses = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,8,REG_colorMap,1,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::GbufferPasses].InitAsDescriptorTable(1,&GbufferPasses,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto TargetTexture = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,5,REG_Target0,2,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::TargetTexture].InitAsDescriptorTable(1,&TargetTexture,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto PermanentTextures = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,5,REG_enviromentCube,3,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::PermanentTextures].InitAsDescriptorTable(1,&PermanentTextures,D3D12_SHADER_VISIBILITY_PIXEL);

	const auto meshBuffer = CD3DX12_DESCRIPTOR_RANGE1(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,4048,REG_colorMap,4,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	rootParameters[eRootBindings::MeshBuffer].InitAsDescriptorTable(1,&meshBuffer,D3D12_SHADER_VISIBILITY_VERTEX);

	const CD3DX12_STATIC_SAMPLER_DESC samplers[]
	{
		CD3DX12_STATIC_SAMPLER_DESC(REG_DefaultSampler,D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR),

		CD3DX12_STATIC_SAMPLER_DESC(REG_BRDFSampler,
									D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
									0,1,D3D12_COMPARISON_FUNC_NEVER,D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,0,0),

		CD3DX12_STATIC_SAMPLER_DESC(REG_shadowCmpSampler,
									D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									D3D12_TEXTURE_ADDRESS_MODE_BORDER,
									0,1,D3D12_COMPARISON_FUNC_GREATER_EQUAL,D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,-D3D12_FLOAT32_MAX,D3D12_FLOAT32_MAX)
	};

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
	rootSignatureDescription.Init_1_1(NumRootParameters,rootParameters,(unsigned)std::size(samplers),samplers,rootSignatureFlags);

	m_RootSignature->SetRootSignatureDesc(rootSignatureDescription.Desc_1_1,GPU::m_FeatureData);
}

void PSOCache::InitMipmapSignature()
{
	//auto device = GPU::m_Device;
	//D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	//featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	//if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE,&featureData,sizeof(featureData))))
	//{
	//	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	//}
	//
	//{
	//	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//	rootSignatureDesc.Init(0,nullptr,0,nullptr,D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	//
	//	ComPtr<ID3DBlob> signature;
	//	ComPtr<ID3DBlob> error;
	//	Helpers::ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc,D3D_ROOT_SIGNATURE_VERSION_1,&signature,&error));
	//	Helpers::ThrowIfFailed(GPU::m_Device->CreateRootSignature(
	//		0,
	//		signature->GetBufferPointer(),
	//		signature->GetBufferSize(),
	//		IID_PPV_ARGS(&m_MipRootSignature->GetRootSignature())
	//	));
	//}
	//
	//
	//CD3DX12_DESCRIPTOR_RANGE1 srcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,1,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	//CD3DX12_DESCRIPTOR_RANGE1 outMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,4,0,0,D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
	//
	//CD3DX12_ROOT_PARAMETER1 rootParameters[GenerateMips::NumRootParameters];
	//rootParameters[GenerateMips::GenerateMipsCB].InitAsConstants(sizeof(GenerateMipsCB) / 4,0);
	//rootParameters[GenerateMips::SrcMip].InitAsDescriptorTable(1,&srcMip);
	//rootParameters[GenerateMips::OutMip].InitAsDescriptorTable(1,&outMip);
	//
	//CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
	//	0,
	//	D3D12_FILTER_MIN_MAG_MIP_LINEAR,
	//	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	//	D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
	//	D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	//);
	//
	//CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//rootSignatureDesc.Init_1_1(GenerateMips::NumRootParameters,rootParameters,1,&linearClampSampler);
	//
	//m_MipRootSignature->SetRootSignatureDesc(rootSignatureDesc.Desc_1_1,featureData.HighestVersion);
}

void PSOCache::InitRootSignature()
{
	m_RootSignature = std::make_shared<GPURootSignature>();
	m_MipRootSignature = std::make_shared<GPURootSignature>();

	InitDefaultSignature();
	InitMipmapSignature();

}

std::unique_ptr<PSO> PSO::CreatePSO(
	const std::filesystem::path& vertexShader,
	const std::filesystem::path& pixelShader,
	const std::span<const DXGI_FORMAT> renderTargetFormat,
	DXGI_FORMAT depthStencilFormat,
	const CD3DX12_BLEND_DESC& desc,
	std::wstring_view name
)
{
	auto pso = std::make_unique<PSO>();
	pso->m_Device = GPU::m_Device;
	pso->m_numRenderTargets = static_cast<int>(renderTargetFormat.size()); 

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
		CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC BlendDesc;
	}  stream;

	pso->m_renderTargets.resize(renderTargetFormat.size());
	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = static_cast<unsigned>(renderTargetFormat.size());
	for (size_t i = 0; i < renderTargetFormat.size(); i++)
	{
		pso->m_renderTargets[i].AllocateTexture(
			{ GPU::m_Width, GPU::m_Height },
			std::format("RenderLayer {}",i),
			renderTargetFormat[i],
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS
		);
		rtvFormats.RTFormats[i] = pso->m_renderTargets[i].GetResource()->GetDesc().Format;
	}

	stream.pRootSignature = PSOCache::m_RootSignature->GetRootSignature().Get();
	stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;


	if (AssetManager::Get().ForceLoadAsset<ShipyardShader>(vertexShader.string(),pso->m_vs))
	{
		stream.VS = CD3DX12_SHADER_BYTECODE(pso->m_vs->GetBlob());
	}


	if (AssetManager::Get().ForceLoadAsset<ShipyardShader>(pixelShader.string(),pso->m_ps))
	{
		stream.PS = CD3DX12_SHADER_BYTECODE(pso->m_ps->GetBlob());
	}

	stream.RTVFormats = rtvFormats;
	stream.DSVFormat = depthStencilFormat;
	stream.BlendDesc = desc;

	auto depthStencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT());
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	stream.DepthStencilState = depthStencil;



	const D3D12_PIPELINE_STATE_STREAM_DESC psoDescStreamDesc = {
			sizeof(PipelineStateStream), &stream
	};
	Helpers::ThrowIfFailed(GPU::m_Device->CreatePipelineState(&psoDescStreamDesc,IID_PPV_ARGS(&pso->m_PipelineState)));
	pso->m_PipelineState->SetName(name.data());

	return pso;
}

ComPtr<ID3D12PipelineState> PSO::GetPipelineState() const
{
	return m_PipelineState;
}
