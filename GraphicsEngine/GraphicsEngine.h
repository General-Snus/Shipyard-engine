#pragma once
#include <memory> 
#include <wrl.h>

#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <GraphicsEngine/Shaders/Registers.h>

#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/G_Buffer.h"
#include "Rendering/Buffers/ConstantBuffer.h" 

#include "InterOp/RHI.h"

#include <AssetManager/Objects/BaseAssets/TextureAsset.h>
#include <AssetManager/Objects/BaseAssets/MeshAsset.h>

#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp> 


using namespace Microsoft::WRL; 

class GraphicsEngine
{
	friend class GraphicCommandBase; 
private:
	FrameBuffer myFrameBuffer;
	ObjectBuffer myObjectBuffer;
	LineBuffer myLineBuffer;
	LightBuffer myLightBuffer;
	G_Buffer myG_Buffer;

	GraphicsCommandList ShadowCommandList;
	GraphicsCommandList DeferredCommandList;
	GraphicsCommandList OverlayCommandList;
	//std::vector<std::unique_ptr<GraphicCommandBase>> ShadowCommandList;
	//std::vector<std::unique_ptr<GraphicCommandBase>> DeferredCommandList;
	//std::vector<std::unique_ptr<GraphicCommandBase>> OverlayCommandList;

	SIZE myWindowSize{ 0,0 };
	HWND myWindowHandle{};
	CU::Vector4<float> myBackgroundColor;

	std::shared_ptr<Texture> myBackBuffer;
	std::shared_ptr<Texture> myDepthBuffer; 

	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;

	std::shared_ptr<Shader> defaultVS;
	std::shared_ptr<Shader> defaultPS;

	//Debug
	ComPtr<ID3D11Buffer> myLineVertexBuffer;
	ComPtr<ID3D11Buffer> myLineIndexBuffer;
	std::shared_ptr<Shader> debugLinePS;
	std::shared_ptr<Shader> debugLineVS;


	std::shared_ptr<Texture> BRDLookUpTable;
	std::shared_ptr<TextureHolder> defaultTexture;
	std::shared_ptr<TextureHolder> defaultNormalTexture;
	std::shared_ptr<TextureHolder> defaultMatTexture;
	std::shared_ptr<TextureHolder> defaultEffectTexture;
	std::shared_ptr<TextureHolder> defaultCubeMap;

	std::shared_ptr<Mesh> defaultMesh;
	std::shared_ptr<Material> defaultMaterial;

	ComPtr<ID3D11SamplerState> myDefaultSampleState;
	ComPtr<ID3D11SamplerState> myShadowSampleState;
	ComPtr<ID3D11SamplerState> myBRDFSampleState;

	ComPtr<ID3D11BlendState> AlphaBlendState;
	ComPtr<ID3D11BlendState> AdditiveBlendState;
	// We're a container singleton, no instancing this outside the class.
	GraphicsEngine() = default;

public:

	static GraphicsEngine& Get() { inline static GraphicsEngine myInstance; return myInstance; }

	/**
	 * Initializes the Graphics Engine with the specified settings.
	 * @param windowHandle The window that will contain this Graphics Engine.
	 * @param enableDeviceDebug If DirectX should write debug output in the Output.
	 */
	bool Initialize(HWND windowHandle, bool enableDeviceDebug);

	bool SetupDebugDrawline(bool& retFlag);

	void SetupDefaultVariables();

	void SetupBRDF();

	void SetLoggingWindow(HANDLE aHandle);

	/**
	 * Prepares the next frame for rendering by resetting states and clearing all render targets.
	 */
	void BeginFrame();

	/**
	 * Finishes a rendered frame and swaps the screen buffers to display it on screen.
	 */
	void EndFrame();

	/**
	 * Renders the current scene to the BackBuffer.
	 */
	void RenderFrame(float aDeltaTime, double aTotalTime);
	CU::Vector4<float>& GetBackgroundColor() { return myBackgroundColor; }

	template<typename CommandClass,typename ...Args>
	FORCEINLINE void ShadowCommands(Args... args)
	{
		ShadowCommandList.AddCommand<CommandClass>(args...);
	}

	template<typename CommandClass,typename ...Args>
	FORCEINLINE void DeferredCommand(Args... args)
	{
		DeferredCommandList.AddCommand<CommandClass>(args ...);
	}

	template<typename CommandClass,typename ...Args>
	FORCEINLINE void OverlayCommands(Args... args)
	{
		OverlayCommandList.AddCommand<CommandClass>(args ...);
	}


	[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
	[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }

	FORCEINLINE std::shared_ptr<Shader> GetDefaultVSShader() const { return defaultVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDefaultPSShader() const { return defaultPS; }

	Texture* DepthBuffer()const { return myDepthBuffer.get(); };
	Texture* BackBuffer()const { return myBackBuffer.get(); };

	FORCEINLINE std::shared_ptr<Shader> GetDebugLineVS() const { return debugLineVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDebugLinePS() const { return debugLinePS; }

	FORCEINLINE ComPtr<ID3D11BlendState> GetAlphaBlendState() const { return AlphaBlendState; }
	FORCEINLINE ComPtr<ID3D11BlendState> GetAdditiveBlendState() const { return AdditiveBlendState; }

	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture() const { return defaultTexture; }
	FORCEINLINE std::shared_ptr<Material> GetDefaultMaterial() const { return defaultMaterial; }
	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture(eTextureType type) const
	{
		switch (type)
		{
		case eTextureType::ColorMap:
			return defaultTexture;

		case eTextureType::NormalMap:
			return defaultNormalTexture;

		case eTextureType::MaterialMap:
			return defaultMatTexture;

		case eTextureType::EffectMap:
			return defaultEffectTexture;

		case eTextureType::CubeMap:
			return defaultCubeMap;

		default:
			return defaultTexture;
		}
	}
};
//template<typename CommandClass, typename ...Args>
//FORCEINLINE void GraphicsEngine::ShadowCommands(Args ... arguments)
//{
//	this->ShadowCommandList.AddCommand<CommandClass>(arguments ...);
//}
//template<typename CommandClass, typename ...Args>
//FORCEINLINE void GraphicsEngine::DeferredCommand(Args ... arguments)
//{
//	this->DeferredCommandList.AddCommand<CommandClass>(arguments ...);
//}
//template<typename CommandClass, typename ...Args>
//FORCEINLINE void GraphicsEngine::OverlayCommands(Args ... arguments)
//{
//	this->OverlayCommandList.AddCommand<CommandClass>(arguments ...);
//} 