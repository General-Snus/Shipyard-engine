#pragma once
#include "BaseAsset.h" // fuck you for bad naming
#include <DirectX/Shipyard/Gpu_fwd.h>
#include <dxcapi.h>

class ShipyardShader : public AssetBase // <---
{
  public:
    MYLIB_REFLECTABLE();
    ShipyardShader(const std::filesystem::path &aFilePath);
    void Init() override;

    void SetShader(const ComPtr<IDxcBlob> &aShader);
    IDxcBlob *GetBlob() const;
    LPVOID GetBufferPtr();
    size_t GetBlobSize() const;
    bool InspectorView() override;
    std::shared_ptr<TextureHolder> GetEditorIcon() override;

  private:
    HRESULT CompileShader(LPCWSTR srcFile, LPCWSTR entryPoint, LPCWSTR profile, IDxcBlob **blob);
    std::filesystem::path m_ShaderName;
    ComPtr<IDxcBlob> m_Blob = nullptr;
};

REFL_AUTO(type(ShipyardShader))
