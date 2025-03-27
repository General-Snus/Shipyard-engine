#pragma once
#include "BaseAsset.h"
#include "Tools/Utilities/Ref.h"
#include <dxcapi.h>
 
class TextureHolder;

class ShipyardShader : public AssetBase {
public:
	ReflectableTypeRegistration();
	ShipyardShader(const std::filesystem::path& aFilePath);
	void Init() override;

	void SetShader(const Ref<IDxcBlob>& aShader);
	IDxcBlob* GetBlob() const;
	LPVOID GetBufferPtr();
	size_t GetBlobSize() const;
	bool InspectorView() override;
	std::shared_ptr<TextureHolder> GetEditorIcon() override;

	//static HRESULT CompileShader(const std::filesystem::path& path,LPCWSTR entryPoint,LPCWSTR profile,IDxcBlob** blob);
	static HRESULT CompileShader(const char* shader,const wchar_t* entryPoint,const wchar_t* target,
		IDxcBlob** blob);
private:
	std::filesystem::path m_ShaderName;
	Ref<IDxcBlob> m_Blob{};
};

REFL_AUTO(type(ShipyardShader))
