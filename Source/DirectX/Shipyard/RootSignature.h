#pragma once   


enum eRootBindings
{
	frameBuffer,         // ConstantBuffer<Mat> MatCB : register(b0);
	objectBuffer,         // ConstantBuffer<Material> objectBuffer : register( b0, space1 );
	Textures,         // Texture2D DiffuseTexture : register( t2 );
	NumRootParameters,
	materialBuffer,  // ConstantBuffer<LightProperties> materialBuffer : register( b1 );
	PointLights,        // StructuredBuffer<PointLight> PointLights : register( t0 );
	SpotLights          // StructuredBuffer<SpotLight> SpotLights : register( t1 );
};
//
//class GPURootParameter
//{
//	friend class RootSignature;
//public:
//
//	GPURootParameter()
//	{
//		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
//	}
//
//	~GPURootParameter()
//	{
//		Clear();
//	}
//
//	void Clear()
//	{
//		if (m_RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
//			delete[] m_RootParam.DescriptorTable.pDescriptorRanges;
//
//		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
//	}
//
//	void InitAsConstants(UINT Register,UINT NumDwords,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
//	{
//		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
//		m_RootParam.ShaderVisibility = Visibility;
//		m_RootParam.Constants.Num32BitValues = NumDwords;
//		m_RootParam.Constants.ShaderRegister = Register;
//		m_RootParam.Constants.RegisterSpace = Space;
//	}
//
//	void InitAsConstantBuffer(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
//	{
//		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//		m_RootParam.ShaderVisibility = Visibility;
//		m_RootParam.Descriptor.ShaderRegister = Register;
//		m_RootParam.Descriptor.RegisterSpace = Space;
//	}
//
//	void InitAsBufferSRV(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
//	{
//		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
//		m_RootParam.ShaderVisibility = Visibility;
//		m_RootParam.Descriptor.ShaderRegister = Register;
//		m_RootParam.Descriptor.RegisterSpace = Space;
//	}
//
//	void InitAsBufferUAV(UINT Register,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
//	{
//		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
//		m_RootParam.ShaderVisibility = Visibility;
//		m_RootParam.Descriptor.ShaderRegister = Register;
//		m_RootParam.Descriptor.RegisterSpace = Space;
//	}
//
//	void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE Type,UINT Register,UINT Count,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL,UINT Space = 0)
//	{
//		InitAsDescriptorTable(1,Visibility);
//		SetTableRange(0,Type,Register,Count,Space);
//	}
//
//	void InitAsDescriptorTable(UINT RangeCount,D3D12_SHADER_VISIBILITY Visibility = D3D12_SHADER_VISIBILITY_ALL)
//	{
//		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//		m_RootParam.ShaderVisibility = Visibility;
//		m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
//		m_RootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
//	}
//
//	void SetTableRange(UINT RangeIndex,D3D12_DESCRIPTOR_RANGE_TYPE Type,UINT Register,UINT Count,UINT Space = 0)
//	{
//		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_RootParam.DescriptorTable.pDescriptorRanges + RangeIndex);
//		range->RangeType = Type;
//		range->NumDescriptors = Count;
//		range->BaseShaderRegister = Register;
//		range->RegisterSpace = Space;
//		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//	}
//
//	const D3D12_ROOT_PARAMETER& operator() (void) const { return m_RootParam; }
//
//
//protected:
//
//	D3D12_ROOT_PARAMETER m_RootParam;
//};

class GPURootSignature
{
	friend class GPU;
public:
	GPURootSignature();
	GPURootSignature(
		const D3D12_ROOT_SIGNATURE_DESC1& rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion
	);

	virtual ~GPURootSignature();

	void Destroy();

	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const
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