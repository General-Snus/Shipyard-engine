#pragma once 
#include <Engine/GraphicsEngine/InterOp/ShaderInfo.h>
#include "BaseAsset.h" // fuck you for bad naming

struct ID3D10Blob;
class ShipyardShader : public AssetBase  // <--- 
{
public:
	ShipyardShader(const std::filesystem::path& aFilePath);
	void Init() override;

	void SetShader(const ComPtr<ID3DBlob>& aShader);
	ShaderType GetShaderType() const;
	ID3DBlob* GetBlob() const;
	LPVOID GetBufferPtr();
	size_t GetBlobSize() const;
	const ShaderInfo& GetShaderInfo() const;
private:
	std::filesystem::path m_ShaderName;
	ComPtr<ID3DBlob> myBlob = nullptr;
	ShaderInfo myShaderInfo{};
};

