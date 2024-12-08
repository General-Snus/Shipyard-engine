#include "AssetManager.pch.h"

#include "../ShipyardShader.h"
#include <d3dcompiler.h>
#include <dxcapi.h>
#include "DirectX/DX12/Graphics/Helpers.h"

static inline ComPtr<IDxcCompiler> pCompiler;
static inline ComPtr<IDxcLibrary>  pLibrary;
static inline uint32_t             codePage = CP_UTF8;

ShipyardShader::ShipyardShader(const std::filesystem::path& aFilePath) : AssetBase(aFilePath)
{
	m_ShaderName = aFilePath.filename();
}

void ShipyardShader::Init()
{
	if (!pCompiler)
	{
		DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), &pCompiler);
	}
	if (!pLibrary)
	{
		DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), &pLibrary);
	}

	isBeingLoaded = true;

	if (!is_regular_file(AssetPath))
	{
		LOGGER.Warn("Failed to load shader at: " + AssetPath.string());
		isBeingLoaded = false;
		isLoadedComplete = false;
		return;
	}

	if ((AssetPath.extension() == ".hlsl"))
	{
#define shaderVersionVS L"vs_6.5"
#define shaderVersionGS L"gs_6.5"
#define shaderVersionPS L"ps_6.5"
#define shaderVersionCS L"vs_6.5"

		const std::string stem = AssetPath.stem().string();
		if (stem.size() > 2)
		{
			std::string lastCharacter = (stem.substr(stem.size() - 2));

			std::ranges::transform(lastCharacter, lastCharacter.begin(),
			                       [](auto a) { return static_cast<char>(::tolower(a)); });
			if (lastCharacter == "vs")
			{
				CompileShader(AssetPath.wstring().c_str(), L"main", shaderVersionVS, &m_Blob);
			}
			else if (lastCharacter == "gs")
			{
				CompileShader(AssetPath.wstring().c_str(), L"main", shaderVersionGS, &m_Blob);
			}
			else if (lastCharacter == "ps")
			{
				CompileShader(AssetPath.wstring().c_str(), L"main", shaderVersionPS, &m_Blob);
			}
			else if (lastCharacter == "cs")
			{
				CompileShader(AssetPath.wstring().c_str(), L"main", shaderVersionCS, &m_Blob);
			}
		}
	}

	// Helpers::ThrowIfFailed(D3DReadFileToBlob(AssetPath.wstring().c_str(), &m_Blob));

	ComPtr<IDxcBlobEncoding> pSourceBlob;
	const HRESULT            hr = pLibrary->CreateBlobFromFile(AssetPath.wstring().c_str(), &codePage, &pSourceBlob);
	m_Blob = pSourceBlob;

	if (FAILED(hr))
	{
		LOGGER.Err("Shader blob compilation failed");
	}
	isBeingLoaded = false;
	isLoadedComplete = true;
}

HRESULT ShipyardShader::CompileShader(_In_ LPCWSTR        srcFile, _In_ LPCWSTR entryPoint, _In_ LPCWSTR profile,
                                      _Outptr_ IDxcBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
	{
		return E_INVALIDARG;
	}

	ComPtr<IDxcBlobEncoding>    pSourceBlob;
	ComPtr<IDxcOperationResult> result;

	// Load shader source code into a blob
	if (FAILED(pLibrary->CreateBlobFromFile(AssetPath.wstring().c_str(), &codePage, &pSourceBlob)))
	{
		return E_FAIL;
	}

	// Compile the shader
	pCompiler->Compile(pSourceBlob.Get(), AssetPath.stem().wstring().c_str(), entryPoint, profile, nullptr, 0, nullptr,
	                   0, nullptr, &result);
	// Check for compile errors
	HRESULT hr;
	result->GetStatus(&hr);
	if (FAILED(hr))
	{
		ComPtr<IDxcBlobEncoding> pErrors;
		result->GetErrorBuffer(&pErrors);
		LOGGER.Warn(("Shader Compilation failed with errors:\n%s", static_cast<char*>(pErrors->GetBufferPointer())));
		return hr;
	}
	result->GetResult(blob);

	return hr;
}

void ShipyardShader::SetShader(const ComPtr<IDxcBlob>& aShader)
{
	m_Blob = aShader;
}

IDxcBlob* ShipyardShader::GetBlob() const
{
	return m_Blob.Get();
}

LPVOID ShipyardShader::GetBufferPtr()
{
	return m_Blob->GetBufferPointer();
}

size_t ShipyardShader::GetBlobSize() const
{
	return m_Blob->GetBufferSize();
}

bool ShipyardShader::InspectorView()
{
	if (!AssetBase::InspectorView())
	{
		return false;
	}

	// Shader specific information,

	// Shader type, not changable? Or compile as what it was loaded as? If

	// Open button,

	// TODO Default serialized shader varible change here, suit with markdown from hlsl file, not today just for future

	//

	ImGui::Text("Shader Name: %s", m_ShaderName.string().c_str());
	ImGui::Text("Shader byte size: %i", m_Blob->GetBufferSize());

	return true;
}

std::shared_ptr<TextureHolder> ShipyardShader::GetEditorIcon()
{
	return ENGINE_RESOURCES.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
}
