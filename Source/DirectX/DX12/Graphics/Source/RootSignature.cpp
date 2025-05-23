#include "DirectXHeader.pch.h"

#include "Graphics/RootSignature.h"
#include "Graphics/GPU.h"

GPURootSignature::GPURootSignature()
	: m_RootSignatureDesc{}
	, m_NumDescriptorsPerTable{ 0 }
	, m_SamplerTableBitMask(0)
	, m_DescriptorTableBitMask(0)
{}

GPURootSignature::GPURootSignature(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc,
								   D3D_ROOT_SIGNATURE_VERSION        rootSignatureVersion)
	: m_RootSignatureDesc{}
	, m_NumDescriptorsPerTable{ 0 }
	, m_SamplerTableBitMask(0)
	, m_DescriptorTableBitMask(0)
{
	SetRootSignatureDesc(rootSignatureDesc, rootSignatureVersion);
}

GPURootSignature::~GPURootSignature()
{
	Destroy();
}

void GPURootSignature::Destroy()
{
	for (UINT i = 0; i < m_RootSignatureDesc.NumParameters; ++i)
	{
		const D3D12_ROOT_PARAMETER1& rootParameter = m_RootSignatureDesc.pParameters[i];
		if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete[] rootParameter.DescriptorTable.pDescriptorRanges;
		}
	}

	delete[] m_RootSignatureDesc.pParameters;
	m_RootSignatureDesc.pParameters = nullptr;
	m_RootSignatureDesc.NumParameters = 0;

	delete[] m_RootSignatureDesc.pStaticSamplers;
	m_RootSignatureDesc.pStaticSamplers = nullptr;
	m_RootSignatureDesc.NumStaticSamplers = 0;

	m_DescriptorTableBitMask = 0;
	m_SamplerTableBitMask = 0;

	memset(m_NumDescriptorsPerTable, 0, sizeof(m_NumDescriptorsPerTable));
}

void GPURootSignature::SetRootSignatureDesc(const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc,
											D3D_ROOT_SIGNATURE_VERSION        rootSignatureVersion)
{
	Destroy();

	const auto device = GPUInstance.m_Device;

	const UINT             numParameters = rootSignatureDesc.NumParameters;
	D3D12_ROOT_PARAMETER1* pParameters = numParameters > 0 ? new D3D12_ROOT_PARAMETER1[numParameters] : nullptr;

	const D3D12_ROOT_SIGNATURE_FLAGS flags = rootSignatureDesc.Flags;
	m_RootSignatureDesc.Flags = flags;

	for (UINT i = 0; i < numParameters; ++i)
	{
		const D3D12_ROOT_PARAMETER1& rootParameter = rootSignatureDesc.pParameters[i];
		pParameters[i] = rootParameter;

		if (rootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			const UINT               numDescriptorRanges = rootParameter.DescriptorTable.NumDescriptorRanges;
			D3D12_DESCRIPTOR_RANGE1* pDescriptorRanges = numDescriptorRanges > 0
				? new D3D12_DESCRIPTOR_RANGE1[numDescriptorRanges]
				: nullptr;

			memcpy(pDescriptorRanges, rootParameter.DescriptorTable.pDescriptorRanges,
				   sizeof(D3D12_DESCRIPTOR_RANGE1) * numDescriptorRanges);

			pParameters[i].DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
			pParameters[i].DescriptorTable.pDescriptorRanges = pDescriptorRanges;

			// Set the bit mask depending on the type of descriptor table.
			if (numDescriptorRanges > 0)
			{
				switch (pDescriptorRanges[0].RangeType)
				{
				case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
					m_DescriptorTableBitMask |= (1 << i);
					break;
				case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
					m_SamplerTableBitMask |= (1 << i);
					break;
				}
			}

			// Count the number of descriptors in the descriptor table.
			for (UINT j = 0; j < numDescriptorRanges; ++j)
			{
				m_NumDescriptorsPerTable[i] += pDescriptorRanges[j].NumDescriptors;
			}
		}
	}

	m_RootSignatureDesc.NumParameters = numParameters;
	m_RootSignatureDesc.pParameters = pParameters;

	const UINT                 numStaticSamplers = rootSignatureDesc.NumStaticSamplers;
	D3D12_STATIC_SAMPLER_DESC* pStaticSamplers = numStaticSamplers > 0
		? new D3D12_STATIC_SAMPLER_DESC[numStaticSamplers]
		: nullptr;

	if (pStaticSamplers)
	{
		memcpy(pStaticSamplers, rootSignatureDesc.pStaticSamplers,
			   sizeof(D3D12_STATIC_SAMPLER_DESC) * numStaticSamplers);
	}

	m_RootSignatureDesc.NumStaticSamplers = numStaticSamplers;
	m_RootSignatureDesc.pStaticSamplers = pStaticSamplers;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc;
	versionRootSignatureDesc.Init_1_1(numParameters, pParameters, numStaticSamplers, pStaticSamplers, flags);

	// Serialize the root signature.
	Ref<ID3DBlob> rootSignatureBlob;
	Ref<ID3DBlob> errorBlob;
	const auto hr = D3DX12SerializeVersionedRootSignature(&versionRootSignatureDesc,
		rootSignatureVersion, rootSignatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr))
	{
		const std::string errorString = static_cast<char*>(errorBlob->GetBufferPointer());
		LOGGER.Err(errorString);
		throw std::exception(errorString.c_str());
	}

	// Create the root signature.
	Helpers::ThrowIfFailed(device->CreateRootSignature(
		0,
		rootSignatureBlob->GetBufferPointer(),
		rootSignatureBlob->GetBufferSize(),
		IID_GRAPHICS_PPV_ARGS(m_RootSignature.GetAddressOf())));

	m_RootSignature->SetName(L"Default root signature");
}

uint32_t GPURootSignature::GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const
{
	uint32_t descriptorTableBitMask = 0;
	switch (descriptorHeapType)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		descriptorTableBitMask = m_DescriptorTableBitMask;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		descriptorTableBitMask = m_SamplerTableBitMask;
		break;
	}

	return descriptorTableBitMask;
}

uint32_t GPURootSignature::GetNumDescriptors(uint32_t rootIndex) const
{
	return m_NumDescriptorsPerTable[rootIndex];
}
