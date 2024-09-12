#include "Engine/AssetManager/AssetManager.pch.h"

#include "../ShipyardShader.h"
#include "DirectX/Shipyard/Helpers.h"
#include <d3dcompiler.h>

ShipyardShader::ShipyardShader(const std::filesystem::path &aFilePath) : AssetBase(aFilePath)
{
    m_ShaderName = aFilePath.filename();
}

void ShipyardShader::Init()
{
    isBeingLoaded = true;

    if (!std::filesystem::is_regular_file(AssetPath))
    {
        Logger::Warn("Failed to load shader at: " + AssetPath.string());
        isBeingLoaded = false;
        isLoadedComplete = false;
        return;
    }

    ID3DBlob *errorBlob = nullptr;
    if (AssetPath.extension() == ".hlsl")
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
        flags |= D3DCOMPILE_DEBUG;
#endif

        CompileShader(AssetPath.wstring().c_str(), "main", "vs_6_5", &myBlob);
    }

    Helpers::ThrowIfFailed(D3DReadFileToBlob(AssetPath.wstring().c_str(), &myBlob));

    isBeingLoaded = false;
    isLoadedComplete = true;
}

HRESULT ShipyardShader::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile,
                                      _Outptr_ ID3DBlob **blob)
{
    if (!srcFile || !entryPoint || !profile || !blob)
        return E_INVALIDARG;

    *blob = nullptr;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
#endif

    const D3D_SHADER_MACRO defines[] = {"EXAMPLE_DEFINE", "1", NULL, NULL};

    ID3DBlob *shaderBlob = nullptr;
    ID3DBlob *errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile, flags, 0,
                                    &shaderBlob, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            OutputDebugStringA((char *)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }

        if (shaderBlob)
            shaderBlob->Release();

        return hr;
    }

    *blob = shaderBlob;

    return hr;
}

void ShipyardShader::SetShader(const ComPtr<ID3DBlob> &aShader)
{
    myBlob = aShader;
}

ID3DBlob *ShipyardShader::GetBlob() const
{
    return myBlob.Get();
}
LPVOID ShipyardShader::GetBufferPtr()
{
    return myBlob->GetBufferPointer();
}
size_t ShipyardShader::GetBlobSize() const
{
    return myBlob->GetBufferSize();
}
