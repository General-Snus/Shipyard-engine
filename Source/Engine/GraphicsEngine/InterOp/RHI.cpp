#include "GraphicsEngine.pch.h"
#include "RHI.h"
#include "comdef.h"
#include <fstream>

#include "DDSTextureLoader11.h"
#include "../Rendering/Vertex.h"
#include "../Rendering/Buffers/ConstantBuffer.h"
#include "Rendering/Texture.h" 

#include "d3d11shader.h"
#include "d3dcompiler.h"
#include "../Objects/Shader.h"
#include "ShaderInfo.h"
#include <iostream>
#include "d3d11_2.h"
//#include "Rendering/Commands/GraphicsCommand.h"

template<typename T, typename STRINGTYPE>
inline static T string_cast(const STRINGTYPE& someString) = delete;
template<typename T, typename CHARTYPE>
inline static T string_cast(const CHARTYPE* someString) = delete;

template<>
inline std::string string_cast<std::string>(const std::wstring& someString)
{
	const int sLength = static_cast<int>(someString.length());
	if(sLength == 0)
		return std::string();
	const int len = WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, 0, 0, 0, 0);
	std::string result(len, L'\0');
	WideCharToMultiByte(CP_ACP, 0, someString.c_str(), -1, result.data(), len, 0, 0);
	return result;
}

template<>
inline std::wstring string_cast<std::wstring>(const std::string& someString)
{
	const int sLength = static_cast<int>(someString.length());
	if(sLength == 0)
		return std::wstring();
	const int len = MultiByteToWideChar(CP_ACP, 0, someString.c_str(), sLength, 0, 0);
	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, someString.c_str(), -1, result.data(), len);
	return result;
}

template<>
inline std::string string_cast<std::string>(const wchar_t* someString)
{
	const std::wstring str = someString;
	return string_cast<std::string>(str);
}

template<>
inline std::wstring string_cast<std::wstring>(const char* someString)
{
	const std::string str = someString;
	return string_cast<std::wstring>(str);
}

void RHI::ReportError(HRESULT aHresult, const std::wstring& aMessage)
{
	const _com_error error(aHresult);
	const LPCWSTR errorMessage = error.ErrorMessage();
	std::wcerr << aMessage + L" " + errorMessage;
}

bool RHI::Initialize(HWND aWindowHandle, bool enableDeviceDebug, Texture* outBackBuffer, Texture* outDepthBuffer)
{
	assert(outBackBuffer && "Please initialize the Texture Object before calling this function!");
	assert(outDepthBuffer && "Please initialize the Texture Object before calling this function!");

	HRESULT result = E_FAIL;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = aWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;

	// More efficient use on Win 8+ but cannot be used on Win 7.
	// Requires Buffercount to be 2.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// If we don't set this DX will lock the refresh date to the monitor
	// refresh rate (even if we don't use V-Sync).
	// It essentially means that we may restart drawing a frame even if
	// doing so would result in a half-drawn frame being put on the screen,
	// i.e. so called screen tearing.
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
	swapChainDesc.BufferCount = 2;

	result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		enableDeviceDebug ? D3D11_CREATE_DEVICE_DEBUG : 0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&mySwapChain,
		&Device,
		nullptr,
		&Context
	);

	if (FAILED(result))
	{
		ReportError(result, L"Failed to create Device and Swap Chain!");
		return false;
	}

	// Create the BackBuffer object that our Graphics Engine will use.
	// This is a special case that can't use our own CreateTexture function because
	// DirectX has already created its own representation via D3D11CreateDeviceAndSwapChain
	// and we just need a place to contain it for our own purposes.
	//outBackBuffer = std::make_shared<Texture>();
	outBackBuffer->myName = L"GraphicsEngine_BackBuffer";
	outBackBuffer->myBindFlags = D3D11_BIND_RENDER_TARGET;
	outBackBuffer->myUsageFlags = D3D11_USAGE_DEFAULT;
	outBackBuffer->myAccessFlags = 0;

	// Retrieve back buffer texture that was created when we called CreateDeviceAndSwapChain.
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &outBackBuffer->myTexture);
	if (FAILED(result))
	{
		ReportError(result, L"Failed to retrieve backbuffer resource!");
		return false;
	}

	// Create a View for the BackBuffer Texture that allows us to draw on the texture.
	result = RHI::Device->CreateRenderTargetView(outBackBuffer->myTexture.Get(),
		nullptr, outBackBuffer->myRTV.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create RTV for backbuffer!");
		return false;
	}

	// Retrieve the display area in the Window. This is the area
	// we can actually do something with, not including the title
	// bar and border.
	RECT clientRect;
	ZeroMemory(&clientRect, sizeof(RECT));
	GetClientRect(aWindowHandle, &clientRect);
	const unsigned int windowWidth = clientRect.right - clientRect.left;
	const unsigned int windowHeight = clientRect.bottom - clientRect.top;

	myDeviceSize = { windowWidth, windowHeight };

	// We also need a viewport that specifies which area of that Texture to draw on.
	outBackBuffer->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f,
		static_cast<float>(windowWidth), static_cast<float>(windowHeight),
		0.0f, 1.0f });

	// Next we need to describes the scene depth. This will allow proper
	// behavior when our 3D world is projected onto our 2D monitor.
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width =  windowWidth ;
	desc.Height = windowHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// For the Scene Depth we can just use our own function to create a depth texture.
	if(!CreateTexture(outDepthBuffer, L"GraphicsEngine_DepthBuffer", windowWidth, windowHeight,
		DXGI_FORMAT::DXGI_FORMAT_R32_TYPELESS, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL))
	{
		ReportError(E_FAIL, L"Failed to create depth buffer!");
		return false;
	}

	// If we got this far we've initialized DirectX! Yay!

	// Now we should create all states we need to keep track of
	myRasterizerStates[RS_Default] = nullptr;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	Device->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_CullNone].GetAddressOf());

	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;	
	Device->CreateRasterizerState(&rasterizerDesc, myRasterizerStates[RS_Wireframe].GetAddressOf());

	myDepthStates[DS_Default] = nullptr;

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = false;
	Device->CreateDepthStencilState(&depthStencilDesc, myDepthStates[DS_LessEqual].GetAddressOf());

	Context->QueryInterface(__uuidof(ID3DUserDefinedAnnotation), &myAnnotationObject);

	//D3D11_FEATURE_DATA_D3D11_OPTIONS1 data;
 //   RHI::Device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &data, sizeof(D3D11_FEATURE_DATA_D3D11_OPTIONS1));

	//ComPtr<ID3D11DeviceContext2> test;
	//Context.As(&test);

	//ComPtr<ID3D11Device2> testDevice;
	//Device.As(&testDevice);

	//D3D11_BUFFER_DESC poolDesc = {};
	//poolDesc.ByteWidth = 128 MB;
	//poolDesc.Usage = D3D11_USAGE_DEFAULT;
	//poolDesc.MiscFlags = D3D11_RESOURCE_MISC_TILE_POOL;
	////poolDesc.BindFlags = ; Tile Pool cannot have BindFlags.

	//ComPtr<ID3D11Buffer> poolBuffer;
	//HRESULT poolResult = testDevice->CreateBuffer(&poolDesc, nullptr, poolBuffer.GetAddressOf());

	//D3D11_TEXTURE2D_DESC tileDesc = {};
	//tileDesc.Width = 16384;
	//tileDesc.Height = 16384;
	//tileDesc.MipLevels = 1;
	//tileDesc.ArraySize = 1;
	//tileDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//tileDesc.SampleDesc.Count = 1;
	//tileDesc.SampleDesc.Quality = 0;
	//tileDesc.Usage = D3D11_USAGE_DEFAULT;
	//tileDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//tileDesc.CPUAccessFlags = 0;
	//tileDesc.MiscFlags = D3D11_RESOURCE_MISC_TILED;

	//ComPtr<ID3D11Texture2D> tiledTexture;
	//HRESULT tiledTextureResult = testDevice->CreateTexture2D(&tileDesc, nullptr, tiledTexture.GetAddressOf());

	//test->UpdateTileMappings()

	return true;
}

bool RHI::UnInitialize()
{
	Device.Reset();
	Context.Reset();
	mySwapChain.Reset();

	return true;
}

void RHI::Present(unsigned aSyncInterval)
{
	mySwapChain->Present(aSyncInterval, DXGI_PRESENT_ALLOW_TEARING);
}

bool RHI::CreateVertexBufferInternal(ComPtr<ID3D11Buffer>& outVxBuffer, const uint8_t* aVertexDataPtr, size_t aNumVertices, size_t aVertexSize)
{
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(aNumVertices * aVertexSize);
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubresourceData{};
	vertexSubresourceData.pSysMem = aVertexDataPtr;

	const HRESULT result = RHI::Device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, outVxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create vertex buffer!");
		return false;
	}

	return true;
}

bool RHI::CreateDynamicVertexBuffer(ComPtr<ID3D11Buffer>& outVxBuffer, size_t aMaxNumberOfVertices, size_t aVertexTypeSize)
{
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = static_cast<UINT>(aMaxNumberOfVertices * aVertexTypeSize);
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	const HRESULT result = RHI::Device->CreateBuffer(&vertexBufferDesc, nullptr, outVxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create dynamic vertex buffer!");
		return false;
	}

	return true;
}

bool RHI::CreateDynamicIndexBuffer(ComPtr<ID3D11Buffer>& outIxBuffer, size_t aMaxNumberOfIndices)
{
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(aMaxNumberOfIndices * sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	const HRESULT result = RHI::Device->CreateBuffer(&indexBufferDesc, nullptr, outIxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create index buffer!");
		return false;
	}

	return true;
}

bool RHI::CreateIndexBuffer(ComPtr<ID3D11Buffer>& outIxBuffer, const std::vector<unsigned>& anIndexList)
{
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = static_cast<UINT>(anIndexList.size() * sizeof(unsigned int));
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexSubresourceData{};
	indexSubresourceData.pSysMem = &anIndexList[0];

	const HRESULT result = RHI::Device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, outIxBuffer.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create index buffer!");
		return false;
	}

	return true;
}

bool RHI::CreateConstantBuffer(ComPtr<ID3D11Buffer>& outCBuffer, size_t aSize)
{
	// Max CBuffer size per buffer in bytes.
	assert(aSize <= 65536);
	if(!RHI::Device)
	{
		ReportError(E_FAIL, L"Failed to create a constant buffer, no Device found! Did you for get to Initialize RHI?");
		return false;
	}

	D3D11_BUFFER_DESC bufferDescription = { 0 };
	bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bufferDescription.ByteWidth = static_cast<unsigned>(aSize);
	const HRESULT result = RHI::Device->CreateBuffer(&bufferDescription, nullptr, outCBuffer.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create constant buffer!");
		return false;
	}

	return true;
}

void RHI::UpdateConstantBufferData(const ConstantBufferBase& aBuffer)
{
	HRESULT result = S_FALSE;
	D3D11_MAPPED_SUBRESOURCE bufferData;

	ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	result = RHI::Context->Map(aBuffer.myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
	if (FAILED(result))
	{
		ReportError(result, L"Failed to map constant buffer when trying to update data!");
	}

	memcpy_s(bufferData.pData, aBuffer.myDataSize, aBuffer.myDataPtr, aBuffer.myDataSize);
	RHI::Context->Unmap(aBuffer.myBuffer.Get(), 0);
}

bool RHI::CreateSamplerState(ComPtr<ID3D11SamplerState>& outSamplerState, const D3D11_SAMPLER_DESC& aDescription)
{
	const HRESULT result = Device->CreateSamplerState(&aDescription, outSamplerState.GetAddressOf());
	if(FAILED(result))
	{
		ReportError(result, L"Failed to create sampler state!");
		return false;
	}

	return true;
}

void RHI::SetSamplerState(const ComPtr<ID3D11SamplerState>& aSamplerState, unsigned aSlot)
{
	Context->PSSetSamplers(aSlot, 1, aSamplerState.GetAddressOf());
}

bool RHI::CreateBlendState(ComPtr<ID3D11BlendState>& outBlendState, const D3D11_BLEND_DESC& aDescription)
{
	const HRESULT result = Device->CreateBlendState(&aDescription, outBlendState.GetAddressOf());
	if(FAILED(result))
	{
		ReportError(result, L"Failed to create blend state!");
		return false;
	}

	return true;
}

void RHI::SetBlendState(const ComPtr<ID3D11BlendState>& aBlendState, const std::array<float, 4>& aBlendFactor,
	unsigned aSamplerMask)
{
	Context->OMSetBlendState(aBlendState.Get(), aBlendFactor.data(), aSamplerMask);
}

bool RHI::LoadVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader>& outVxShader, ComPtr<ID3D11InputLayout>& outInputLayout, const std::wstring& aFileName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc)
{
	std::ifstream vsFile;
	vsFile.open(aFileName, std::ios::binary);
	const std::vector<uint8_t> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	vsFile.close();

	return CreateVertexShaderAndInputLayout(outVxShader, outInputLayout, anInputLayoutDesc, vsData.data(), vsData.size());
}

bool RHI::CreateVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader>& outVxShader,
	ComPtr<ID3D11InputLayout>& outInputLayout, const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc,
	const BYTE* someShaderData, size_t aShaderDataSize)
{
	HRESULT result = Device->CreateVertexShader(someShaderData, aShaderDataSize, nullptr, &outVxShader);
	if (FAILED(result))
	{
		ReportError(result, L"Failed to load vertex shader from the specified file!");
	}

	if(!outInputLayout)
	{
		result = Device->CreateInputLayout(
			anInputLayoutDesc.data(),
			static_cast<UINT>(anInputLayoutDesc.size()),
			someShaderData,
			aShaderDataSize,
			outInputLayout.GetAddressOf()
		);

		if (FAILED(result))
		{
			ReportError(result, L"Failed to create input layout for the specified vertex shader!");
			return false;
		}
	}

	return true;
}

bool RHI::CreateInputLayout(ComPtr<ID3D11InputLayout>& outInputLayout,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc, const BYTE* someShaderData, size_t aShaderDataSize)
{
	if(!outInputLayout)
	{
		const HRESULT result = Device->CreateInputLayout(
			anInputLayoutDesc.data(),
			static_cast<UINT>(anInputLayoutDesc.size()),
			someShaderData,
			aShaderDataSize,
			outInputLayout.GetAddressOf()
		);

		if (FAILED(result))
		{
			ReportError(result, L"Failed to create input layout for the specified vertex shader!");
			return false;
		}
	}

	return true;
}

bool RHI::CreateVertexShader(ComPtr<ID3D11VertexShader>& outVxShader, const BYTE* someShaderData,
                             size_t aShaderDataSize)
{
	const HRESULT result = Device->CreateVertexShader(someShaderData, aShaderDataSize, nullptr, &outVxShader);
	if (FAILED(result))
	{
		ReportError(result, L"Failed to load vertex shader from the specified file!");
		return false;
	}

	return true;
}

bool RHI::CreatePixelShader(ComPtr<ID3D11PixelShader>& outPxShader, const BYTE* someShaderData, size_t aShaderDataSize)
{
	const HRESULT result = Device->CreatePixelShader(someShaderData, aShaderDataSize, nullptr, outPxShader.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to load pixel shader from the specified file!");
		return false;
	}

	return true;
}

void RHI::ReflectShaderVariableMember(const std::wstring& aDomain, size_t& anOffset,
	ID3D11ShaderReflectionType* aMemberType, const D3D11_SHADER_TYPE_DESC* aMemberTypeDesc,
	ShaderInfo::ConstantBufferInfo& inoutBufferInfo)
{
	if(aMemberTypeDesc->Members > 0)
	{
		for(unsigned m = 0; m < aMemberTypeDesc->Members; m++)
		{
			ID3D11ShaderReflectionType* childMemberType = aMemberType->GetMemberTypeByIndex(m);

			D3D11_SHADER_TYPE_DESC childMemberTypeDesc;
			ZeroMemory(&childMemberTypeDesc, sizeof(D3D11_SHADER_TYPE_DESC));
			childMemberType->GetDesc(&childMemberTypeDesc);

			const std::wstring childMemberName = string_cast<std::wstring>(aMemberType->GetMemberTypeName(m));
			const std::wstring newDomain = aDomain.empty() ? childMemberName : aDomain + L"." + childMemberName;

			ReflectShaderVariableMember(newDomain, anOffset, childMemberType, &childMemberTypeDesc, inoutBufferInfo);
		}
	}
	else
	{
		ShaderInfo::VariableInfo varInfo;
		varInfo.Name = aDomain;
		varInfo.Type = string_cast<std::wstring>(aMemberTypeDesc->Name);
		varInfo.Size = static_cast<unsigned long long>(aMemberTypeDesc->Columns) *
			static_cast<unsigned long long>(aMemberTypeDesc->Rows) * sizeof(float);
		varInfo.Offset = anOffset;
		anOffset += varInfo.Size;
		inoutBufferInfo.VariableNameToIndex.emplace(varInfo.Name, inoutBufferInfo.Variables.size());
		inoutBufferInfo.Variables.emplace_back(varInfo);
	}
}

void RHI::ReflectShaderVariable(const std::wstring& aDomain, size_t& anOffset, ID3D11ShaderReflectionVariable* aVariable, ShaderInfo::ConstantBufferInfo& inoutBufferInfo)
{
	aDomain;
	// Get all the info about this particular variable.
	// I.e. its description...
	D3D11_SHADER_VARIABLE_DESC variableDesc;
	ZeroMemory(&variableDesc, sizeof(D3D11_SHADER_VARIABLE_DESC));
	aVariable->GetDesc(&variableDesc);
	// ...and it's type object and its description.
	ID3D11ShaderReflectionType* variableType = aVariable->GetType();
	D3D11_SHADER_TYPE_DESC variableTypeDesc;
	ZeroMemory(&variableTypeDesc, sizeof(D3D11_SHADER_TYPE_DESC));
	variableType->GetDesc(&variableTypeDesc);

	// Now we should know what this is.
	if(variableTypeDesc.Members > 0)
	{
		// Start a Domain for this Variable
		const std::wstring memberDomain = string_cast<std::wstring>(variableDesc.Name);
		ReflectShaderVariableMember(memberDomain, anOffset, variableType, &variableTypeDesc, inoutBufferInfo);	
	}
	else
	{
		ShaderInfo::VariableInfo varInfo;
		varInfo.Name = string_cast<std::wstring>(variableDesc.Name);
		varInfo.Type = string_cast<std::wstring>(variableTypeDesc.Name);
		varInfo.Size = variableDesc.Size;
		varInfo.Offset = anOffset;
		anOffset += variableDesc.Size;
		inoutBufferInfo.VariableNameToIndex.emplace(varInfo.Name, inoutBufferInfo.Variables.size());
		inoutBufferInfo.Variables.emplace_back(varInfo);
	}
}

bool RHI::ReflectShader(ShaderInfo& outShRefl, const BYTE* someShaderData, size_t aShaderDataSize)
{
	ComPtr<ID3D11ShaderReflection> Data;
	const HRESULT result = D3DReflect(someShaderData, aShaderDataSize, IID_ID3D11ShaderReflection, reinterpret_cast<void**>(Data.GetAddressOf()));
	if(FAILED(result))
	{
		ReportError(result, L"Failed to reflect the provided shader!");
		return false;
	}

	ComPtr<ID3D11ShaderReflection> shaderReflection = Data;
	D3D11_SHADER_DESC shaderDesc;
	ZeroMemory(&shaderDesc, sizeof(D3D11_SHADER_DESC));
	shaderReflection->GetDesc(&shaderDesc);

	outShRefl.InstructionCount = shaderDesc.InstructionCount;
	outShRefl.ConstantBuffers.reserve(shaderDesc.ConstantBuffers);
	outShRefl.Type = static_cast<ShaderType>(D3D11_SHVER_GET_TYPE(shaderDesc.Version));

	if(outShRefl.Type == ShaderType::VertexShader)
	{
		//// We need to extract the Input Layout too.
		//std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		//for(unsigned p = 0; p < shaderDesc.InputParameters; p++)
		//{
		//	D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		//	shaderReflection->GetInputParameterDesc(p, &paramDesc);

		//	D3D11_INPUT_ELEMENT_DESC element;
		//	element.SemanticName = paramDesc.SemanticName;
		//	element.SemanticIndex = paramDesc.SemanticIndex;
		//	element.InputSlot = paramDesc.Register;
		//	element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		//	element.InputSlotClass = paramDesc.
		//}
	}

	for(unsigned c = 0; c < shaderDesc.ConstantBuffers; c++)
	{
		ShaderInfo::ConstantBufferInfo bufferInfo;
		ID3D11ShaderReflectionConstantBuffer* cbufferReflection = shaderReflection->GetConstantBufferByIndex(c);

		D3D11_SHADER_BUFFER_DESC cbufferDesc;
		D3D11_SHADER_INPUT_BIND_DESC cbufferBindDesc;
		ZeroMemory(&cbufferDesc, sizeof(D3D11_SHADER_BUFFER_DESC));
		ZeroMemory(&cbufferBindDesc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));

		cbufferReflection->GetDesc(&cbufferDesc);
		shaderReflection->GetResourceBindingDescByName(cbufferDesc.Name, &cbufferBindDesc);
		
		bufferInfo.Size = cbufferDesc.Size;
		std::string bufferName = cbufferDesc.Name;
		bufferInfo.Name = string_cast<std::wstring>(bufferName);
		bufferInfo.Slot = cbufferBindDesc.BindPoint;

		bufferInfo.Variables.reserve(cbufferDesc.Variables);
		bufferInfo.VariableNameToIndex.reserve(cbufferDesc.Variables);
		size_t dataOffset = 0;
		for(unsigned v = 0; v < cbufferDesc.Variables; v++)
		{
			ID3D11ShaderReflectionVariable* varReflection = cbufferReflection->GetVariableByIndex(v);
			ReflectShaderVariable(L"", dataOffset, varReflection, bufferInfo);
		}

		outShRefl.ConstantBufferNameToIndex.emplace(bufferInfo.Name, outShRefl.ConstantBuffers.size());
		outShRefl.ConstantBuffers.emplace_back(std::move(bufferInfo));
	}

	for(unsigned r = 0; r < shaderDesc.BoundResources; r++)
	{
		D3D11_SHADER_INPUT_BIND_DESC resourceBindDesc;
		ZeroMemory(&resourceBindDesc, sizeof(D3D11_SHADER_INPUT_BIND_DESC));
		shaderReflection->GetResourceBindingDesc(r, &resourceBindDesc);
		switch(resourceBindDesc.Type)
		{
		case D3D_SIT_TEXTURE:
			{
				ShaderInfo::TextureInfo textureInfo;
				textureInfo.Name = string_cast<std::wstring>(resourceBindDesc.Name);
				textureInfo.Slot = resourceBindDesc.BindPoint;
				switch(resourceBindDesc.Dimension)
				{
				case D3D_SRV_DIMENSION_TEXTURE1D:
					{
						textureInfo.Type = TextureType::Texture1D;
					}
				case D3D_SRV_DIMENSION_TEXTURE2D:
					{
						textureInfo.Type = TextureType::Texture2D;
					}
				case D3D_SRV_DIMENSION_TEXTURE3D:
					{
						textureInfo.Type = TextureType::Texture3D;
					}
				default:
					{
						textureInfo.Type = TextureType::Unknown;
					}
				}

				outShRefl.TextureNameToIndex.emplace(textureInfo.Name, outShRefl.Textures.size());
				outShRefl.Textures.emplace_back(std::move(textureInfo));
			}
			break;
		case D3D_SIT_SAMPLER:
			{
				ShaderInfo::SamplerInfo samplerInfo;
				samplerInfo.Name = string_cast<std::wstring>(resourceBindDesc.Name);
				samplerInfo.Slot = resourceBindDesc.BindPoint;

				outShRefl.SamplerNameToIndex.emplace(samplerInfo.Name, outShRefl.Samplers.size());
				outShRefl.Samplers.emplace_back(std::move(samplerInfo));
			}
			break;
		default:
			break;
		}
	}

	return true;
}

bool RHI::CreateGeometryShader(ComPtr<ID3D11GeometryShader>& outGeoShader, const BYTE* someShaderData, size_t aShaderDataSize)
{
	const HRESULT result = Device->CreateGeometryShader(someShaderData, aShaderDataSize, nullptr, outGeoShader.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to load geometry shader from the specified file!");
		return false;
	}

	return true;
}

bool RHI::LoadPixelShader(ComPtr<ID3D11PixelShader>& outPxShader, const std::wstring& aFileName)
{
	std::ifstream psFile;
	psFile.open(aFileName, std::ios::binary);
	const std::vector<uint8_t> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();

	return CreatePixelShader(outPxShader, psData.data(), psData.size());
}

bool RHI::LoadTexture(Texture* outTexture, const std::wstring& aFileName)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	//outTexture = std::make_shared<Texture>();
	outTexture->myName = aFileName;
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;

	HRESULT result = DirectX::CreateDDSTextureFromFile(
		Device.Get(),
		aFileName.c_str(),
		outTexture->myTexture.GetAddressOf(),
		outTexture->mySRV.GetAddressOf()
	);

	if (FAILED(result))
	{
		ReportError(result, L"Failed to load the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	result = Device->CreateShaderResourceView(outTexture->myTexture.Get(), nullptr, outTexture->mySRV.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	std::wstring textureName = aFileName;
	if(const size_t pos = textureName.find_last_of(L'\\'); pos != std::wstring::npos)
	{
		textureName = textureName.substr(pos + 1);
	}

	textureName = textureName.substr(0, textureName.size() - 4);
	outTexture->myName = textureName;

	return true;
}

bool RHI::LoadShader(Shader* outShader, const std::wstring& aFileName)
{
	assert(outShader && "Please initialize the Shader Object before calling this function!");
	std::ifstream shFile;
	shFile.open(aFileName, std::ios::binary);
	const std::vector<uint8_t> shData = { std::istreambuf_iterator<char>(shFile), std::istreambuf_iterator<char>() };
	shFile.close();

	std::wstring shaderName = aFileName;
	if(const size_t pos = shaderName.find_last_of(L'\\'); pos != std::wstring::npos)
	{
		shaderName = shaderName.substr(pos + 1);
	}

	shaderName = shaderName.substr(0, shaderName.size() - 4);

	return LoadShaderFromMemory(outShader, shaderName, shData.data(), shData.size());
}
 
bool RHI::LoadShaderFromMemory(Shader* outShader, const std::wstring& aName, const BYTE* someShaderData,
	size_t aShaderDataSize)
{
	assert(outShader && "Please initialize the Shader Object before calling this function!");
	outShader->myName = aName;
	outShader->myBlob = new unsigned char[aShaderDataSize];
	outShader->myBlobSize = aShaderDataSize;
	memcpy_s(outShader->myBlob, outShader->myBlobSize, someShaderData, aShaderDataSize);

	if(ReflectShader(outShader->myShaderInfo, outShader->myBlob, outShader->myBlobSize))
	{
		switch(outShader->GetShaderType())
		{
		case ShaderType::VertexShader:
			{
				ComPtr<ID3D11VertexShader> vsShader;
				CreateVertexShader(vsShader, outShader->myBlob, outShader->myBlobSize);
				outShader->myShaderObject = vsShader;
			}
			break;
		case ShaderType::PixelShader:
			{
				ComPtr<ID3D11PixelShader> psShader;
				CreatePixelShader(psShader, outShader->myBlob, outShader->myBlobSize);
				outShader->myShaderObject = psShader;
			}
			break;
		case ShaderType::GeometryShader:
			{
				ComPtr<ID3D11GeometryShader> gsShader;
				CreateGeometryShader(gsShader, outShader->myBlob, outShader->myBlobSize);
				outShader->myShaderObject = gsShader;
			}
			break;
		}

		return true;
	}

	return false;
} 

bool RHI::LoadTextureFromMemory(Texture* outTexture, const std::wstring& aName, const BYTE* someImageData, size_t anImageDataSize, const D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	//outTexture = std::make_shared<Texture>();
	outTexture->myName = aName;
	outTexture->myBindFlags = D3D11_BIND_SHADER_RESOURCE;
	outTexture->myUsageFlags = D3D11_USAGE_DEFAULT;
	outTexture->myAccessFlags = 0;

	HRESULT result = DirectX::CreateDDSTextureFromMemory(
		Device.Get(),
		someImageData,
		anImageDataSize,
		outTexture->myTexture.GetAddressOf(),
		outTexture->mySRV.GetAddressOf()
	);

	if (FAILED(result))
	{
		ReportError(result, L"Failed to load the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	result = Device->CreateShaderResourceView(outTexture->myTexture.Get(), aSRVDesc, outTexture->mySRV.GetAddressOf());
	if (FAILED(result))
	{
		ReportError(result, L"Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	return true;
}

void RHI::ConfigureInputAssembler(unsigned aTopology, const ComPtr<ID3D11Buffer>& aVxBuffer, const ComPtr<ID3D11Buffer>& anIxBuffer,
                                  unsigned aVertexStride, const ComPtr<ID3D11InputLayout>& anInputLayout)
{
	Context->IASetInputLayout(anInputLayout.Get());
	Context->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(aTopology));
	constexpr unsigned vxOffset = 0;
	Context->IASetVertexBuffers(0, 1, aVxBuffer.GetAddressOf(), &aVertexStride, &vxOffset);
	DXGI_FORMAT indexBufferFormat = DXGI_FORMAT_R32_UINT;
	if(anIxBuffer == nullptr)
	{
		indexBufferFormat = DXGI_FORMAT_UNKNOWN;
	}
	Context->IASetIndexBuffer(anIxBuffer.Get(), indexBufferFormat, 0);
}

void RHI::SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader)
{
	Context->VSSetShader(aVertexShader.Get(), nullptr, 0);
}

void RHI::SetVertexShader(const Shader* aVertexShader)
{
	if(!aVertexShader)
	{
		Context->VSSetShader(nullptr, nullptr, 0);
		return;
	}
	assert(aVertexShader && aVertexShader->GetShaderType() == ShaderType::VertexShader && "That's not a Vertex Shader!");
	ComPtr<ID3D11VertexShader> vsShader;
	aVertexShader->GetShader().As(&vsShader);
	Context->VSSetShader(vsShader.Get(), nullptr, 0);
}

void RHI::SetGeometryShader(const ComPtr<ID3D11GeometryShader>& aGeometryShader)
{
	Context->GSSetShader(aGeometryShader.Get(), nullptr, 0);
}

void RHI::SetGeometryShader(const Shader* aGeometryShader)
{
	if(!aGeometryShader)
	{
		Context->GSSetShader(nullptr, nullptr, 0);
		return;
	}
	assert(aGeometryShader && aGeometryShader->GetShaderType() == ShaderType::GeometryShader && "That's not a Geometry Shader!");
	ComPtr<ID3D11GeometryShader> gsShader;
	aGeometryShader->GetShader().As(&gsShader);
	Context->GSSetShader(gsShader.Get(), nullptr, 0);
}

void RHI::SetPixelShader(const ComPtr<ID3D11PixelShader>& aPixelShader)
{
	Context->PSSetShader(aPixelShader.Get(), nullptr, 0);
}

void RHI::SetPixelShader(const Shader* aPixelShader)
{
	if(!aPixelShader)
	{
		Context->PSSetShader(nullptr, nullptr, 0);
		return;
	}
	assert(aPixelShader && aPixelShader->GetShaderType() == ShaderType::PixelShader && "That's not a Pixel Shader!");
	ComPtr<ID3D11PixelShader> psShader;
	aPixelShader->GetShader().As(&psShader);
	Context->PSSetShader(psShader.Get(), nullptr, 0);
}


void RHI::SetRasterizerState(const RasterizerState aState)
{
	Context->RSSetState(myRasterizerStates[aState].Get());
}

void RHI::SetDepthState(const DepthState aState)
{
	Context->OMSetDepthStencilState(myDepthStates[aState].Get(), 0);
}

void RHI::SetConstantBuffer(UINT aPipelineStages, unsigned aSlot, const ConstantBufferBase& aBuffer)
{
	if(aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		Context->VSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
	}

	if(aPipelineStages & PIPELINE_STAGE_GEOMETERY_SHADER)
	{
		Context->GSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
	}

	if(aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		Context->PSSetConstantBuffers(aSlot, 1, aBuffer.myBuffer.GetAddressOf());
	}
}

void RHI::DrawIndexed(unsigned aCount)
{
	Context->DrawIndexed(aCount, 0, 0);
}

void RHI::Draw(unsigned aCount)
{
	Context->Draw(aCount, 0);
}

bool RHI::CreateTexture(Texture* outTexture, const std::wstring& aName,
	size_t aWidth, size_t aHeight, UINT aFormat, D3D11_USAGE someUsageFlags,
	UINT someBindFlags,	UINT someCpuAccessFlags)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	outTexture->myName = aName;
	outTexture->myBindFlags =  someBindFlags;
	outTexture->myUsageFlags = someUsageFlags;
	outTexture->myAccessFlags = someCpuAccessFlags;

	HRESULT result = S_FALSE;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = static_cast<unsigned>(aWidth);
	desc.Height = static_cast<unsigned>(aHeight);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = static_cast<DXGI_FORMAT>(aFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = someUsageFlags;
	desc.BindFlags = someBindFlags;
	desc.CPUAccessFlags = someCpuAccessFlags;
	desc.MiscFlags = 0;

	result = Device->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(outTexture->myTexture.GetAddressOf()));
	if(FAILED(result))
	{
		ReportError(result, L"Failed to create the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	if (someBindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
		depthDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT;
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		result = Device->CreateDepthStencilView(outTexture->myTexture.Get(), &depthDesc, outTexture->myDSV.GetAddressOf());
		if (FAILED(result))
		{
			ReportError(result, L"Failed to create a depth stencil view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	if (someBindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// If we want a SRV and we're also a Depth Stencil we need
		// to specify these settings for the SRV to make sense.
		D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc = {};
		depthSRVDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT;
		depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depthSRVDesc.Texture2D.MipLevels = desc.MipLevels;

		result = Device->CreateShaderResourceView(outTexture->myTexture.Get(),
			(someBindFlags & D3D11_BIND_DEPTH_STENCIL) ? &depthSRVDesc : nullptr, outTexture->mySRV.GetAddressOf());
		if(FAILED(result))
		{
			ReportError(result, L"Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	if (someBindFlags & D3D11_BIND_RENDER_TARGET)
	{
		result = Device->CreateRenderTargetView(outTexture->myTexture.Get(), nullptr, outTexture->myRTV.GetAddressOf());
		if (FAILED(result))
		{
			ReportError(result, L"Failed to create a render target view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	outTexture->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f,
		static_cast<float>(aWidth), static_cast<float>(aHeight),
		0.0f, 1.0f });

	return true;
}

bool RHI::CreateTextureCube(Texture* outTexture, const std::wstring& aName, size_t aWidth, size_t aHeight, UINT aFormat,
	D3D11_USAGE someUsageFlags, UINT someBindFlags, UINT someCpuAccessFlags)
{
	assert(outTexture && "Please initialize the Texture Object before calling this function!");
	outTexture->myName = aName;
	outTexture->myBindFlags =  someBindFlags;
	outTexture->myUsageFlags = someUsageFlags;
	outTexture->myAccessFlags = someCpuAccessFlags;

	HRESULT result = S_FALSE;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = static_cast<unsigned>(aWidth);
	desc.Height = static_cast<unsigned>(aHeight);
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = static_cast<DXGI_FORMAT>(aFormat);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = someUsageFlags;
	desc.BindFlags = someBindFlags;
	desc.CPUAccessFlags = someCpuAccessFlags;
	desc.MiscFlags = D3D10_RESOURCE_MISC_TEXTURECUBE;

	result = Device->CreateTexture2D(&desc, nullptr, reinterpret_cast<ID3D11Texture2D**>(outTexture->myTexture.GetAddressOf()));
	if(FAILED(result))
	{
		ReportError(result, L"Failed to create the requested texture! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
		return false;
	}

	if (someBindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc = {};
		depthDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT;
		depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		depthDesc.Texture2DArray.FirstArraySlice = 0;
		depthDesc.Texture2DArray.ArraySize = 6;

		result = Device->CreateDepthStencilView(outTexture->myTexture.Get(), &depthDesc, outTexture->myDSV.GetAddressOf());
		if (FAILED(result))
		{
			ReportError(result, L"Failed to create a depth stencil view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	if (someBindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// If we want a SRV and we're also a Depth Stencil we need
		// to specify these settings for the SRV to make sense.
		D3D11_SHADER_RESOURCE_VIEW_DESC depthSRVDesc = {};
		depthSRVDesc.Format = aFormat == DXGI_FORMAT_R32G8X24_TYPELESS ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT;
		depthSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		depthSRVDesc.TextureCube.MipLevels = desc.MipLevels;
		depthSRVDesc.TextureCube.MostDetailedMip = 0;

		result = Device->CreateShaderResourceView(outTexture->myTexture.Get(),
			(someBindFlags & D3D11_BIND_DEPTH_STENCIL) ? &depthSRVDesc : nullptr, outTexture->mySRV.GetAddressOf());
		if(FAILED(result))
		{
			ReportError(result, L"Failed to create a shader resource view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	if (someBindFlags & D3D11_BIND_RENDER_TARGET)
	{
		result = Device->CreateRenderTargetView(outTexture->myTexture.Get(), nullptr, outTexture->myRTV.GetAddressOf());
		if (FAILED(result))
		{
			ReportError(result, L"Failed to create a render target view! Please check the DirectX Debug Output for more information. If there is none make sure you set enableDeviceDebug to True.");
			return false;
		}
	}

	outTexture->myViewport = D3D11_VIEWPORT({ 0.0f, 0.0f,
		static_cast<float>(aWidth), static_cast<float>(aHeight),
		0.0f, 1.0f });

	return true;
}


void RHI::SetTextureResource(UINT aPipelineStages, unsigned aSlot, const Texture* aTexture)
{
	if(aTexture && (aTexture->myBindFlags & D3D11_BIND_SHADER_RESOURCE) == false)
	{
		std::throw_with_nested(std::invalid_argument("Attempted to set a write only texture as Shader Resource!"));
	}

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		if(!aTexture)
		{
			ID3D11ShaderResourceView* dummy = nullptr;
			Context->VSSetShaderResources(aSlot, 1, &dummy);	
		}
		else
		{
			Context->VSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());	
		}		
	}

	if (aPipelineStages & PIPELINE_STAGE_GEOMETERY_SHADER)
	{
		if(!aTexture)
		{
			ID3D11ShaderResourceView* dummy = nullptr;
			Context->GSSetShaderResources(aSlot, 1, &dummy);	
		}
		else
		{
			Context->GSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}		
	}

	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		if(!aTexture)
		{
			ID3D11ShaderResourceView* dummy = nullptr;
			Context->PSSetShaderResources(aSlot, 1, &dummy);	
		}
		else
		{
			Context->PSSetShaderResources(aSlot, 1, aTexture->mySRV.GetAddressOf());
		}
	}
}

void RHI::SetTextureResources(UINT aPipelineStages, unsigned aStartSlot,
	const std::vector<std::shared_ptr<Texture>>& aTextureList)
{
	std::vector<ID3D11ShaderResourceView*> mySRVs;
	mySRVs.resize(aTextureList.size());
	for(size_t i = 0; i < aTextureList.size(); i++)
	{
		mySRVs[i] = aTextureList[i] ? aTextureList[i]->mySRV.Get() : nullptr;
	}

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		Context->VSSetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), mySRVs.data());
	}

	if (aPipelineStages & PIPELINE_STAGE_GEOMETERY_SHADER)
	{
		Context->GSGetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), mySRVs.data());
	}

	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		Context->PSSetShaderResources(aStartSlot, static_cast<unsigned>(aTextureList.size()), mySRVs.data());
	}
}

void RHI::RemoveTextureResource(UINT aPipelineStages, unsigned aSlot)
{
	ComPtr<ID3D11ShaderResourceView> empty = nullptr;

	if (aPipelineStages & PIPELINE_STAGE_VERTEX_SHADER)
	{
		Context->VSSetShaderResources(aSlot, 1, empty.GetAddressOf());
	}

	if (aPipelineStages & PIPELINE_STAGE_GEOMETERY_SHADER)
	{
		Context->GSGetShaderResources(aSlot, 1, empty.GetAddressOf());
	}

	if (aPipelineStages & PIPELINE_STAGE_PIXEL_SHADER)
	{
		Context->PSSetShaderResources(aSlot, 1, empty.GetAddressOf());
	}
}

void RHI::SetRenderTarget(const Texture* aTarget, const Texture* aDepthStencil)
{
	Context->OMSetRenderTargets(aTarget ? 1 : 0, aTarget ? aTarget->myRTV.GetAddressOf() : nullptr, aDepthStencil ? aDepthStencil->myDSV.Get() : nullptr);
	if(aTarget)
	{
		Context->RSSetViewports(1, &aTarget->myViewport);
	}
	else if(aDepthStencil)
	{
		Context->RSSetViewports(1, &aDepthStencil->myViewport);
	}
}

void RHI::SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aTargetList,
	const std::shared_ptr<Texture>& aDepthStencil)
{
	std::vector<ID3D11RenderTargetView*> myRTVs;
	myRTVs.reserve(aTargetList.size());

	for (size_t t = 0; t < aTargetList.size(); t++)
	{
		if(!aTargetList[t] || (aTargetList[t]->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
		{
			std::throw_with_nested(std::invalid_argument("Attempted to set an invalid or read only texture as Render Target!"));
		}
		myRTVs.push_back(aTargetList[t]->myRTV.Get());
	}

	const UINT numRenderTargets = static_cast<UINT>(aTargetList.size());

	Context->OMSetRenderTargets(numRenderTargets, myRTVs.empty() ? nullptr : myRTVs.data(), aDepthStencil ? aDepthStencil->myDSV.Get() : nullptr);
	if(aTargetList[0])
	{
		Context->RSSetViewports(1, &aTargetList[0]->myViewport);
	}
	else if(aDepthStencil)
	{
		Context->RSSetViewports(1, &aDepthStencil->myViewport);
	}
}

void RHI::SetRenderTargets(const std::vector<Texture*>& aTargetList, const Texture* aDepthStencil)
{
	std::vector<ID3D11RenderTargetView*> myRTVs;
	myRTVs.reserve(aTargetList.size());

	for (size_t t = 0; t < aTargetList.size(); t++)
	{
		if(!aTargetList[t] || (aTargetList[t]->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
		{
			std::throw_with_nested(std::invalid_argument("Attempted to set an invalid or read only texture as Render Target!"));
		}
		myRTVs.push_back(aTargetList[t]->myRTV.Get());
	}

	const UINT numRenderTargets = static_cast<UINT>(aTargetList.size());

	Context->OMSetRenderTargets(numRenderTargets, myRTVs.empty() ? nullptr : myRTVs.data(), aDepthStencil ? aDepthStencil->myDSV.Get() : nullptr);
	if(aTargetList[0])
	{
		Context->RSSetViewports(1, &aTargetList[0]->myViewport);
	}
	else if(aDepthStencil)
	{
		Context->RSSetViewports(1, &aDepthStencil->myViewport);
	}
}

void RHI::CopyRegion(const Texture* aSource, const Texture* aDestination, unsigned X,
                     unsigned Y, unsigned Width, unsigned Height)
{
	D3D11_BOX pickBox = {};
	pickBox.left = X;
	pickBox.right = X + Width;
	pickBox.top = Y;
	pickBox.bottom = Y + Height;
	pickBox.front = 0;
	pickBox.back = 1;

	ID3D11Resource* objIdResource = nullptr;
	aSource->myRTV.Get()->GetResource(&objIdResource);
	Context->CopySubresourceRegion(aDestination->myTexture.Get(),
	                               0, 0, 0, 0, objIdResource, 0, &pickBox);
}

std::array<BYTE, 4> RHI::GetPixel(const Texture* aSource, unsigned X, unsigned Y)
{
	X; Y;
	std::array<BYTE, 4> bytes = { 0, 0, 0, 0 };

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Context->Map(aSource->myTexture.Get(), 0, D3D11_MAP_READ, 0, &resource);
	if (resource.pData != nullptr)
	{
		memcpy(bytes.data(), resource.pData, sizeof(BYTE) * 4);
	}
	Context->Unmap(aSource->myTexture.Get(), 0);

	return bytes;
}

void RHI::RemoveTarget()
{
	ComPtr<ID3D11RenderTargetView> currentRTV;
	ComPtr<ID3D11DepthStencilView> currentDSV;
	Context->OMGetRenderTargets(1, currentRTV.GetAddressOf(), currentDSV.GetAddressOf());
	Context->OMSetRenderTargets(0, nullptr, currentDSV.Get());
}

void RHI::ClearRenderTarget(const Texture* aTexture, std::array<float, 4> aClearColor)
{
	if((aTexture->myBindFlags & D3D11_BIND_RENDER_TARGET) == false)
	{
		std::throw_with_nested(std::runtime_error("Attempted to clear a read-only texture!"));
	}
	Context->ClearRenderTargetView(aTexture->myRTV.Get(), aClearColor.data());
}

void RHI::ClearDepthStencil(const Texture* aTexture)
{
	if((aTexture->myBindFlags & D3D11_BIND_DEPTH_STENCIL) == false)
	{
		std::throw_with_nested(std::runtime_error("Attempted to clear depth on a non-depth texture!"));
	}

	RHI::Context->ClearDepthStencilView(aTexture->myDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RHI::BeginEvent(const std::wstring& anEventName)
{
	myAnnotationObject->BeginEvent(anEventName.c_str());
}

void RHI::EndEvent()
{
	myAnnotationObject->EndEvent();
}