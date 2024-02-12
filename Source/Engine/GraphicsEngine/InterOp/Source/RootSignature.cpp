#include "GraphicsEngine.pch.h"
#include "../RootSignature.h"


static std::map< size_t,ComPtr<ID3D12RootSignature> > s_RootSignatureHashMap;
void GPURootSignature::RegisterSampler(UINT aRegister,const D3D12_SAMPLER_DESC& nonStaticSamplerDesc,D3D12_SHADER_VISIBILITY visibility)
{
	assert(m_NumInitializedStaticSamplers < m_NumSamplers);
	D3D12_STATIC_SAMPLER_DESC& StaticSamplerDesc = m_SamplerArray[m_NumInitializedStaticSamplers++];

	StaticSamplerDesc.Filter = nonStaticSamplerDesc.Filter;
	StaticSamplerDesc.AddressU = nonStaticSamplerDesc.AddressU;
	StaticSamplerDesc.AddressV = nonStaticSamplerDesc.AddressV;
	StaticSamplerDesc.AddressW = nonStaticSamplerDesc.AddressW;
	StaticSamplerDesc.MipLODBias = nonStaticSamplerDesc.MipLODBias;
	StaticSamplerDesc.MaxAnisotropy = nonStaticSamplerDesc.MaxAnisotropy;
	StaticSamplerDesc.ComparisonFunc = nonStaticSamplerDesc.ComparisonFunc;
	StaticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	StaticSamplerDesc.MinLOD = nonStaticSamplerDesc.MinLOD;
	StaticSamplerDesc.MaxLOD = nonStaticSamplerDesc.MaxLOD;
	StaticSamplerDesc.ShaderRegister = aRegister;
	StaticSamplerDesc.RegisterSpace = 0;  
}

void GPURootSignature::Reset(UINT NumRootParams,UINT NumStaticSamplers)
{
	if (NumRootParams > 0)
		m_ParamArray.reset(new GPURootParameter[NumRootParams]);
	else
		m_ParamArray = nullptr;
	m_NumParameters = NumRootParams;

	if (NumStaticSamplers > 0)
		m_SamplerArray.reset(new D3D12_STATIC_SAMPLER_DESC[NumStaticSamplers]);
	else
		m_SamplerArray = nullptr;
	m_NumSamplers = NumStaticSamplers;
	m_NumInitializedStaticSamplers = 0;
}

void GPURootSignature::Finalize(const std::wstring& name,D3D12_ROOT_SIGNATURE_FLAGS Flags)
{
	assert(m_NumInitializedStaticSamplers == m_NumSamplers);

	D3D12_ROOT_SIGNATURE_DESC RootDesc;
	RootDesc.NumParameters = m_NumParameters;
	RootDesc.pParameters = (const D3D12_ROOT_PARAMETER*)m_ParamArray.get();
	RootDesc.NumStaticSamplers = m_NumSamplers;
	RootDesc.pStaticSamplers = (const D3D12_STATIC_SAMPLER_DESC*)m_SamplerArray.get();
	RootDesc.Flags = Flags;

	m_DescriptorTableBitMap = 0;
	m_SamplerTableBitMap = 0;

	size_t HashCode = Helpers::HashState(&RootDesc.Flags);
	HashCode = Helpers::HashState(RootDesc.pStaticSamplers,m_NumSamplers,HashCode);

	for (UINT Param = 0; Param < m_NumParameters; ++Param)
	{
		const D3D12_ROOT_PARAMETER& RootParam = RootDesc.pParameters[Param];
		m_DescriptorTableSize[Param] = 0;

		if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			assert(RootParam.DescriptorTable.pDescriptorRanges != nullptr);

			HashCode = Helpers::HashState(RootParam.DescriptorTable.pDescriptorRanges,
				RootParam.DescriptorTable.NumDescriptorRanges,HashCode);

			// We keep track of sampler descriptor tables separately from CBV_SRV_UAV descriptor tables
			if (RootParam.DescriptorTable.pDescriptorRanges->RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
				m_SamplerTableBitMap |= (1 << Param);
			else
				m_DescriptorTableBitMap |= (1 << Param);

			for (UINT TableRange = 0; TableRange < RootParam.DescriptorTable.NumDescriptorRanges; ++TableRange)
				m_DescriptorTableSize[Param] += RootParam.DescriptorTable.pDescriptorRanges[TableRange].NumDescriptors;
		}
		else
			HashCode = Helpers::HashState(&RootParam,1,HashCode);
	}

	ID3D12RootSignature** RSRef = nullptr;
	bool firstCompile = false;
	{
		static std::mutex s_HashMapMutex;
		std::lock_guard<std::mutex> CS(s_HashMapMutex);
		auto iter = s_RootSignatureHashMap.find(HashCode);

		// Reserve space so the next inquiry will find that someone got here first.
		if (iter == s_RootSignatureHashMap.end())
		{
			RSRef = s_RootSignatureHashMap[HashCode].GetAddressOf();
			firstCompile = true;
		}
		else
			RSRef = iter->second.GetAddressOf();
	}

	if (firstCompile)
	{
		ComPtr<ID3DBlob> pOutBlob,pErrorBlob;

		Helpers::ThrowIfSucceded(D3D12SerializeRootSignature(&RootDesc,D3D_ROOT_SIGNATURE_VERSION_1,
		                                                   pOutBlob.GetAddressOf(),pErrorBlob.GetAddressOf()));

		Helpers::ThrowIfSucceded(GPU::m_Device->CreateRootSignature(1,pOutBlob->GetBufferPointer(),pOutBlob->GetBufferSize(),
		                                                            IID_PPV_ARGS(&m_RootSignature)));

		m_RootSignature->SetName(name.c_str());

		s_RootSignatureHashMap[HashCode].Attach(m_RootSignature.Get());
		assert(*RSRef == m_RootSignature.Get());
	}
	else
	{
		while (*RSRef == nullptr)
			std::this_thread::yield();
		m_RootSignature = *RSRef;
	}

	m_Finalized = TRUE;
}
