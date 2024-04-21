#pragma once 

enum eRootBindings
{
	frameBuffer,
	objectBuffer,
	materialBuffer,
	lightBuffer,
	MeshBuffer,
	Textures,
	GbufferPasses,
	TargetTexture,
	PermanentTextures,
	NumRootParameters,
};

struct ID3D12RootSignature;

class GPURootSignature
{
public:
	GPURootSignature();
	GPURootSignature(
		const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion
	);

	virtual ~GPURootSignature();

	void Destroy();

	ComPtr<ID3D12RootSignature> GetRootSignature() const
	{
		return m_RootSignature;
	}

	void SetRootSignatureDesc(
		const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion
	);

	const D3D12_ROOT_SIGNATURE_DESC1& GetRootSignatureDesc() const
	{
		return m_RootSignatureDesc;
	}

	uint32_t GetDescriptorTableBitMask(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType) const;
	uint32_t GetNumDescriptors(uint32_t rootIndex) const;

private:
	D3D12_ROOT_SIGNATURE_DESC1 m_RootSignatureDesc;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	uint32_t m_NumDescriptorsPerTable[32];
	uint32_t m_SamplerTableBitMask;
	uint32_t m_DescriptorTableBitMask;


};