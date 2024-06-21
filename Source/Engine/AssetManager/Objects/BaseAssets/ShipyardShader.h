#pragma once 
#include <DirectX/Shipyard/Gpu_fwd.h> 
#include "BaseAsset.h" // fuck you for bad naming


class ShipyardShader : public AssetBase  // <--- 
{
public:
	MYLIB_REFLECTABLE();
	ShipyardShader(const std::filesystem::path& aFilePath);
	void Init() override;

	void SetShader(const ComPtr<ID3DBlob>& aShader); 
	ID3DBlob* GetBlob() const;
	LPVOID GetBufferPtr();
	size_t GetBlobSize() const; 
private:
	std::filesystem::path m_ShaderName;
	ComPtr<ID3DBlob> myBlob = nullptr; 
};


REFL_AUTO(type(ShipyardShader))