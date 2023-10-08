#include "GraphicsEngine.pch.h"
#include "GraphicsEngine.h"

#include <filesystem>
#include <utility>

#include <Shaders/Include/Default_PS.h>
#include <Shaders/Include/Default_VS.h>

#include <Shaders/Include/Default_C.h>
#include <Shaders/Include/Default_N.h>
#include <Shaders/Include/Default_M.h>
#include <Shaders/Include/Default_FX.h>

#include <Shaders/Include/LineDrawer_PS.h>
#include <Shaders/Include/LineDrawer_VS.h>

#include <Shaders/Include/brdfLUT_PS.h>
#include <Shaders/Include/brdfLUT_VS.h>
#include <Shaders/Registers.h>

#include "Objects/Shader.h"

#include "Rendering/Vertex.h"
#include "Objects/Model.h"
#include "Commands/GraphicCommands.h"
#include <AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/LightComponent.h>

#include "Shaders/Registers.h"
#include <ImGui/imgui.h>


bool GraphicsEngine::Initialize(HWND windowHandle,bool enableDeviceDebug)
{
	GELogger = Logger::Create("GraphicsEngine");

#ifdef _DEBUG
	try
	{
#endif
		GELogger.SetPrintToVSOutput(false);

		myWindowHandle = windowHandle;
		myBackBuffer = std::make_unique<Texture>();
		myDepthBuffer = std::make_unique<Texture>();

		if(!RHI::Initialize(myWindowHandle,
			enableDeviceDebug,
			myBackBuffer.get(),
			myDepthBuffer.get()))
		{
			GELogger.Err("Failed to initialize the RHI!");
			return false;
		}


		SetupDefaultVariables();
		SetupBRDF();

		bool retFlag;
		bool retVal = SetupDebugDrawline(retFlag);
		if(retFlag) return retVal;


		D3D11_BLEND_DESC blendDesc = {};
		D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
		rtBlendDesc.BlendEnable = TRUE;
		rtBlendDesc.SrcBlend = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		if(!RHI::CreateBlendState(AlphaBlendState,blendDesc))
		{
			assert(false);
		}

		rtBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtBlendDesc.DestBlend = D3D11_BLEND_ONE;
		rtBlendDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
		rtBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
		rtBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


		if(!RHI::CreateBlendState(AdditiveBlendState,blendDesc))
		{
			assert(false);
		}


		myLightBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_LightBuffer,myLightBuffer);

		myObjectBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_ObjectBuffer,myObjectBuffer);

		myFrameBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_FrameBuffer,myFrameBuffer);

		myLineBuffer.Initialize();
		RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_LineBuffer,myLineBuffer);

		myG_Buffer.Init();

#ifdef _DEBUG
	}
	catch(const std::exception& e)
	{
		GELogger.LogException(e);
		exit(-1);
	}
#endif 
	return true;
}

bool GraphicsEngine::SetupDebugDrawline(bool& retFlag)
{
	retFlag = true;
	debugLineVS = std::make_shared<Shader>();
	RHI::LoadShaderFromMemory(
		debugLineVS.get(),
		L"LineVertexShader",
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);

	debugLinePS = std::make_shared<Shader>();
	RHI::LoadShaderFromMemory(
		debugLinePS.get(),
		L"LinePixelShader",
		BuiltIn_LineDrawer_PS_ByteCode,
		sizeof(BuiltIn_LineDrawer_PS_ByteCode)
	);

	//Create DebugVertex input layout
	RHI::CreateInputLayout(
		Debug::DebugVertex::InputLayout,
		Debug::DebugVertex::InputLayoutDescription,
		BuiltIn_LineDrawer_VS_ByteCode,
		sizeof(BuiltIn_LineDrawer_VS_ByteCode)
	);

	if(!(
		RHI::CreateDynamicVertexBuffer(myLineVertexBuffer,65536,sizeof(Debug::DebugVertex)) &&
		RHI::CreateDynamicIndexBuffer(myLineIndexBuffer,65536)
		))
	{
		GELogger.Err("Failed to initialize the myLineVertexBuffer!");
		return false;
	}
	retFlag = false;
	return {};
}

void GraphicsEngine::SetupDefaultVariables()
{
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.f;
	samplerDesc.BorderColor[1] = 1.f;
	samplerDesc.BorderColor[2] = 1.f;
	samplerDesc.BorderColor[3] = 1.f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if(!RHI::CreateSamplerState(myDefaultSampleState,samplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	}
	RHI::SetSamplerState(myDefaultSampleState,REG_DefaultSampler);

	D3D11_SAMPLER_DESC shadowSamplerDesc = {};
	shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 0.f;
	shadowSamplerDesc.BorderColor[1] = 0.f;
	shadowSamplerDesc.BorderColor[2] = 0.f;
	shadowSamplerDesc.BorderColor[3] = 0.f;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowSamplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	shadowSamplerDesc.MipLODBias = 0.f;
	shadowSamplerDesc.MaxAnisotropy = 1;

	if(!RHI::CreateSamplerState(myShadowSampleState,shadowSamplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	} 
	RHI::SetSamplerState(myShadowSampleState,REG_shadowCmpSampler);

	// TEMP: Load the default shader programs.
	// This will be done elsewhere later on :).
	RHI::CreateVertexShaderAndInputLayout(
		myVertexShader,
		Vertex::InputLayout,
		Vertex::InputLayoutDefinition,
		BuiltIn_Default_VS_ByteCode,
		sizeof(BuiltIn_Default_VS_ByteCode)
	);
	RHI::CreatePixelShader(
		myPixelShader,
		BuiltIn_Default_PS_ByteCode,
		sizeof(BuiltIn_Default_PS_ByteCode)
	);
	RHI::CreateInputLayout(
		Vertex::InputLayout,
		Vertex::InputLayoutDefinition,
		BuiltIn_Default_VS_ByteCode,
		sizeof(BuiltIn_Default_VS_ByteCode)
	);

	defaultTexture = std::make_shared<TextureHolder>("",eTextureType::ColorMap);
	RHI::LoadTextureFromMemory(
		defaultTexture->GetRawTexture().get(),
		L"Default Color texture",
		BuiltIn_Default_C_ByteCode,
		sizeof(BuiltIn_Default_C_ByteCode)
	);

	defaultNormalTexture = std::make_shared<TextureHolder>("",eTextureType::NormalMap);
	RHI::LoadTextureFromMemory(
		defaultNormalTexture->GetRawTexture().get(),
		L"Default Normal texture",
		BuiltIn_Default_N_ByteCode,
		sizeof(BuiltIn_Default_N_ByteCode)
	);

	defaultMatTexture = std::make_shared<TextureHolder>("",eTextureType::MaterialMap);
	RHI::LoadTextureFromMemory(
		defaultMatTexture->GetRawTexture().get(),
		L"Default material texture",
		BuiltIn_Default_M_ByteCode,
		sizeof(BuiltIn_Default_M_ByteCode)
	);

	defaultEffectTexture = std::make_shared<TextureHolder>("",eTextureType::EffectMap);
	RHI::LoadTextureFromMemory(
		defaultEffectTexture->GetRawTexture().get(),
		L"Default effect texture",
		BuiltIn_Default_FX_ByteCode,
		sizeof(BuiltIn_Default_FX_ByteCode)
	);

	defaultVS = std::make_shared<Shader>();
	defaultPS = std::make_shared<Shader>();

	defaultVS->SetShader(myVertexShader);
	defaultVS->myName = L"Default Vertex Shader";
	defaultPS->SetShader(myPixelShader);
	defaultPS->myName = L"Default Pixel Shader";

	AssetManager::GetInstance().ForceLoadAsset<Material>("Materials/Default.json",defaultMaterial);
	defaultMaterial->SetShader(defaultVS,defaultPS);

	AssetManager::GetInstance().ForceLoadAsset<TextureHolder>("Textures/skansen_cubemap.dds",defaultCubeMap);
	defaultCubeMap->SetTextureType(eTextureType::CubeMap);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_enviromentCube,defaultCubeMap->GetRawTexture().get());

	AssetManager::GetInstance().ForceLoadAsset<Mesh>("default.fbx",defaultMesh);
}

void GraphicsEngine::SetupBRDF()
{
	//Light
	BRDLookUpTable = std::make_shared<Texture>();
	RHI::CreateTexture(BRDLookUpTable.get(),L"brdfLUT",512,512,
		DXGI_FORMAT_R16G16_FLOAT,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
	);
	RHI::ClearRenderTarget(BRDLookUpTable.get());

	ComPtr<ID3D11VertexShader> brdfVS;
	ComPtr<ID3D11PixelShader> brdfPS;

	RHI::CreateVertexShader(
		brdfVS,
		BuiltIn_brdfLUT_VS_ByteCode,
		sizeof(BuiltIn_brdfLUT_VS_ByteCode)
	);

	RHI::CreatePixelShader(
		brdfPS,
		BuiltIn_brdfLUT_PS_ByteCode,
		sizeof(BuiltIn_brdfLUT_PS_ByteCode)
	);
	RHI::SetVertexShader(brdfVS);
	RHI::SetPixelShader(brdfPS);

	RHI::SetRenderTarget(BRDLookUpTable.get(),nullptr);

	RHI::ConfigureInputAssembler(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		nullptr,nullptr,0,nullptr);
	RHI::Draw(4);

	RHI::SetRenderTarget(nullptr,nullptr);
	RHI::SetTextureResource(PIPELINE_STAGE_PIXEL_SHADER,REG_BRDF_LUT_Texture,BRDLookUpTable.get());


	D3D11_SAMPLER_DESC lutsamplerDesc = {};
	lutsamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	lutsamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	lutsamplerDesc.MipLODBias = 0.f;
	lutsamplerDesc.MaxAnisotropy = 1;
	lutsamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	lutsamplerDesc.BorderColor[0] = 1.f;
	lutsamplerDesc.BorderColor[1] = 1.f;
	lutsamplerDesc.BorderColor[2] = 1.f;
	lutsamplerDesc.BorderColor[3] = 1.f;
	lutsamplerDesc.MinLOD = 0;
	lutsamplerDesc.MaxLOD = 0;

	if(!RHI::CreateSamplerState(myBRDFSampleState,lutsamplerDesc))
	{
		GELogger.Log("Sampler state created");
		assert(false);
	}

	RHI::SetSamplerState(myBRDFSampleState,REG_BRDFSampler);

}

void GraphicsEngine::SetLoggingWindow(HANDLE aHandle)
{
	GELogger.SetConsoleHandle(aHandle);
}

void GraphicsEngine::BeginFrame()
{
	// Here we should initialize our frame and clean up from the last one.  
	RHI::ClearRenderTarget(myBackBuffer.get(),myBackgroundColor); 
	RHI::ClearDepthStencil(myDepthBuffer.get());
	myG_Buffer.ClearTargets();
}

void GraphicsEngine::RenderFrame(float aDeltaTime,double aTotalTime)
{ 
	aDeltaTime; aTotalTime;
	DirectionalLight* dirLight;
	std::shared_ptr<Texture> shadowMap;
	Matrix lightMatrix;
	for(auto& i : GameObjectManager::GetInstance().GetAllComponents<cLight>())
	{
		if(i.GetType() == eLightType::Directional)
		{	
			RHI::SetVertexShader(myVertexShader); 
			shadowMap = i.GetShadowMap();
			dirLight = i.GetData<DirectionalLight>().get(); 
			myLightBuffer.Data.myDirectionalLight.Color = dirLight->Color;
			myLightBuffer.Data.myDirectionalLight.Power = dirLight->Power;
			myLightBuffer.Data.myDirectionalLight.Direction = dirLight->Direction; 
			myLightBuffer.Data.myDirectionalLight.projection = dirLight->projection;
			RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_LightBuffer,myLightBuffer);
			RHI::UpdateConstantBufferData(myLightBuffer);

			RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,nullptr); // cant be bound to resources when rendering to it
			RHI::ClearDepthStencil(shadowMap.get()); 
			RHI::SetRenderTarget(nullptr,shadowMap.get());
			GfxCmd_SetFrameBuffer(dirLight->projection,dirLight->lightView,0).Execute();

			for(const auto& command : ShadowCommandList)
			{ 
				command->Execute();
			}
			RHI::SetRenderTarget(nullptr,nullptr); 
		}
	}

	ImGui::Begin("shadowmap");
	ImGui::Image((void*)shadowMap->GetSRV(),ImVec2(1000,1000));
	ImGui::End();


	
	 
	myG_Buffer.SetWriteTargetToBuffer(); //Let all write to textures
	for(const auto& command : DeferredCommandList)
	{
		command->Execute();
	} 
	RHI::SetRenderTarget(myBackBuffer.get(),nullptr);


	//LIGHTS 
	RHI::SetTextureResource(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_dirLightShadowMap,shadowMap.get());
	GfxCmd_SetLightBuffer().Execute(); 

	GfxCmd_DebugLayer().Execute(); 
	RHI::SetBlendState(AlphaBlendState);

	for(const auto& command : this->OverlayCommandList)
	{
		command->Execute();
	}
	RHI::SetBlendState(nullptr); 
}

void GraphicsEngine::EndFrame()
{
	// We finish our frame here and present it on screen.
	RHI::Present(0);
	this->OverlayCommandList.clear();
	this->DeferredCommandList.clear();
	this->ShadowCommandList.clear();
}

