#include "AssetManager.pch.h"

#include "../ShipyardShader.h" 
#include "DirectX/DX12/Graphics/Helpers.h"
#include <d3d12shader.h> 

static inline ComPtr<IDxcCompiler3> pCompiler;
static inline ComPtr<IDxcUtils> pUtils;
static inline ComPtr<IDxcLibrary>  pLibrary;
static inline uint32_t             codePage = CP_UTF8;

ShipyardShader::ShipyardShader(const std::filesystem::path& aFilePath) : AssetBase(aFilePath) {
	m_ShaderName = aFilePath.filename();
}


void initStatics() {
	if(!pCompiler) {
		DxcCreateInstance(CLSID_DxcCompiler,__uuidof(IDxcCompiler3),&pCompiler);
	}
	if(!pUtils) {
		DxcCreateInstance(CLSID_DxcUtils,__uuidof(IDxcUtils),&pUtils);
	}
	if(!pLibrary) {
		DxcCreateInstance(CLSID_DxcLibrary,__uuidof(IDxcLibrary),&pLibrary);
	}
}
void ShipyardShader::Init() {

	initStatics();
	isBeingLoaded = true;

	if(!is_regular_file(AssetPath)) {
		LOGGER.Warn("Failed to load shader at: " + AssetPath.string());
		isBeingLoaded = false;
		isLoadedComplete = false;
		return;
	}

	if((AssetPath.extension() == ".hlsl")) {
#define shaderVersionVS L"vs_6.5"
#define shaderVersionGS L"gs_6.5"
#define shaderVersionPS L"ps_6.5"
#define shaderVersionCS L"vs_6.5"

		const std::string stem = AssetPath.stem().string();
		if(stem.size() > 2) {
			std::string lastCharacter = (stem.substr(stem.size() - 2));

			std::ranges::transform(lastCharacter,lastCharacter.begin(),
				[](auto a) { return static_cast<char>(::tolower(a)); });
			if(lastCharacter == "vs") {
				//CompileShader(AssetPath.wstring().c_str(),L"main",shaderVersionVS,&m_Blob);
				return;
			} else if(lastCharacter == "gs") {
				//CompileShader(AssetPath.wstring().c_str(),L"main",shaderVersionGS,&m_Blob);
				return;
			} else if(lastCharacter == "ps") {
				//CompileShader(AssetPath.wstring().c_str(),L"main",shaderVersionPS,&m_Blob);
				return;
			} else if(lastCharacter == "cs") {
				//CompileShader(AssetPath.wstring().c_str(),L"main",shaderVersionCS,&m_Blob);
				return;
			}
		}
	}

	// Helpers::ThrowIfFailed(D3DReadFileToBlob(AssetPath.wstring().c_str(), &m_Blob));

	Ref<IDxcBlobEncoding> pSourceBlob;
	const HRESULT            hr = pLibrary->CreateBlobFromFile(AssetPath.wstring().c_str(),&codePage,pSourceBlob.GetAddressOf());
	//const HRESULT            hr = pUtils->LoadFile(AssetPath.wstring().c_str(),&codePage,&pSourceBlob);
	m_Blob = pSourceBlob;

	if(FAILED(hr)) {
		LOGGER.Err("Shader blob compilation failed");
	}
	isBeingLoaded = false;
	isLoadedComplete = true;
}
//
//HRESULT ShipyardShader::CompileShader(const std::filesystem::path& path,_In_ LPCWSTR entryPoint,_In_ LPCWSTR profile,
//	_Outptr_ IDxcBlob** blob) {
//	if(!path.empty() || !entryPoint || !profile || !blob) {
//		return E_INVALIDARG;
//	}
//
//	//ComPtr<IDxcBlobEncoding>    pSourceBlob;
//	//ComPtr<IDxcOperationResult> result;
//
//	//// Load shader source code into a blob
//	//if (FAILED(pUtils->LoadFile(path.wstring().c_str(), &codePage, &pSourceBlob)))
//	//{
//	//	return E_FAIL;
//	//}
//
//	//// Compile the shader
//	//pCompiler->Compile(pSourceBlob.Get(),path.stem().wstring().c_str(), entryPoint, profile, nullptr, 0, nullptr,
//	//                   0, nullptr, &result);
//	//// Check for compile errors
//	//HRESULT hr;
//	//result->GetStatus(&hr);
//	//if (FAILED(hr))
//	//{
//	//	ComPtr<IDxcBlobEncoding> pErrors;
//	//	result->GetErrorBuffer(&pErrors);
//	//	LOGGER.Warn(("Shader Compilation failed with errors:\n%s", static_cast<char*>(pErrors->GetBufferPointer())));
//	//	return hr;
//	//}
//	//result->GetResult(blob);
//
//	//return hr;
//}


HRESULT ShipyardShader::CompileShader(const char* shader,const wchar_t* entryPoint,const wchar_t* target,
	IDxcBlob** blob) {

	if(!shader || !entryPoint || !target || !blob) {
		return E_INVALIDARG;
	}

	initStatics();

	auto strLength = strlen(shader);
	DxcBuffer sourceBuffer;
	sourceBuffer.Ptr = shader;
	sourceBuffer.Size = strLength;
	sourceBuffer.Encoding = DXC_CP_ACP;

	std::vector<LPCWSTR> arguments;
	// -E for the entry point (eg. 'main')
	arguments.emplace_back(L"-E");
	arguments.emplace_back(entryPoint);

	arguments.emplace_back(L"-T");
	arguments.emplace_back(target);

	ComPtr<IDxcIncludeHandler> pIncludeHandler;
	pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);


	// Compile the shader  
	ComPtr<IDxcResult> pResults;
	if(FAILED(pCompiler->Compile(&sourceBuffer,arguments.data(),(uint32_t)arguments.size(),pIncludeHandler.Get(),IID_PPV_ARGS(pResults.GetAddressOf())))) {
		// Either an unrecoverable error exception was caught or a failing HRESULT was returned
		// Use fputs to prevent any chance of new allocations
		// Terminate the process
		puts("Internal error or API misuse! Compile Failed\n");
		return 1;
	}

	//
	// Print errors if present.
	//
	ComPtr<IDxcBlobUtf8> pErrors = nullptr;
	// Note that d3dcompiler would return null if no errors or warnings are present.
	// IDxcCompiler3::Compile will always return an error buffer,
	// but its length will be zero if there are no warnings or errors.
	if(SUCCEEDED(pResults->GetOutput(DXC_OUT_ERRORS,IID_PPV_ARGS(&pErrors),nullptr)) &&
		pErrors != nullptr && pErrors->GetStringLength() != 0) {
		const auto str = std::format("Warnings and Errors:\n{}\n",pErrors->GetStringPointer());
		LOGGER.Warn(str);
	}

	//
	// Quit if the compilation failed.
	//
	HRESULT hrStatus;
	if(FAILED(pResults->GetStatus(&hrStatus)) || FAILED(hrStatus)) {
		// Compilation failed, but successful HRESULT was returned.
		// Could reuse the compiler and allocator objects. For simplicity, exit here anyway
		LOGGER.Warn("Compilation Failed\n");
		return 1;
	}

	////
	//// Save shader binary.
	////     
	ComPtr<IDxcBlob> pShader;
	ComPtr<IDxcBlobUtf16> pShaderName = nullptr;
	if(SUCCEEDED(pResults->GetOutput(DXC_OUT_OBJECT,IID_PPV_ARGS(&pShader),&pShaderName)) && pShader != nullptr) {
		//	FILE* fp = NULL;

		//	_wfopen_s(&fp,pShaderName->GetStringPointer(),L"wb");
		//	fwrite(pShader->GetBufferPointer(),pShader->GetBufferSize(),1,fp);
		//	fclose(fp);
	}

	//
	// Save pdb.
	//
	ComPtr<IDxcBlob> pPDB = nullptr;
	ComPtr<IDxcBlobUtf16> pPDBName = nullptr;
	if(SUCCEEDED(pResults->GetOutput(DXC_OUT_PDB,IID_PPV_ARGS(&pPDB),&pPDBName))) {
		FILE* fp = NULL;

		// Note that if you don't specify -Fd, a pdb name will be automatically generated.
		// Use this file name to save the pdb so that PIX can find it quickly.
		_wfopen_s(&fp,pPDBName->GetStringPointer(),L"wb");
		fwrite(pPDB->GetBufferPointer(),pPDB->GetBufferSize(),1,fp);
		fclose(fp);
	}


	//
	// Get separate reflection.
	//
	ComPtr<IDxcBlob> pReflectionData;
	if(SUCCEEDED(pResults->GetOutput(DXC_OUT_REFLECTION,IID_PPV_ARGS(&pReflectionData),nullptr)) &&
		pReflectionData != nullptr) {
		// Optionally, save reflection blob for later here.

		// Create reflection interface.
		DxcBuffer ReflectionData;
		ReflectionData.Encoding = DXC_CP_ACP;
		ReflectionData.Ptr = pReflectionData->GetBufferPointer();
		ReflectionData.Size = pReflectionData->GetBufferSize();

		ComPtr< ID3D12ShaderReflection > pReflection;
		pUtils->CreateReflection(&ReflectionData,IID_PPV_ARGS(&pReflection));

		// Use reflection interface here.

	}

	*blob = pShader.Detach();
	return 0;
}

void ShipyardShader::SetShader(const Ref<IDxcBlob>& aShader) {
	m_Blob = aShader;
}

IDxcBlob* ShipyardShader::GetBlob() const {
	return m_Blob.Get();
}

LPVOID ShipyardShader::GetBufferPtr() {
	return m_Blob->GetBufferPointer();
}

size_t ShipyardShader::GetBlobSize() const {
	return m_Blob->GetBufferSize();
}

bool ShipyardShader::InspectorView() {
	if(!AssetBase::InspectorView()) {
		return false;
	}

	// Shader specific information,

	// Shader type, not changable? Or compile as what it was loaded as? If

	// Open button,

	// TODO Default serialized shader varible change here, suit with markdown from hlsl file, not today just for future

	//

	ImGui::Text("Shader Name: %s",m_ShaderName.string().c_str());
	ImGui::Text("Shader byte size: %i",m_Blob->GetBufferSize());

	return true;
}

std::shared_ptr<TextureHolder> ShipyardShader::GetEditorIcon() {
	return ENGINE_RESOURCES.LoadAsset<TextureHolder>("Textures/Widgets/File.png");
}
