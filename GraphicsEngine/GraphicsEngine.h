#pragma once
#include "GraphicsEngine.pch.h"
#include <memory>
#include "InterOp/RHI.h"
#include <wrl.h>
#include <ThirdParty/CU/CommonUtills/Matrix4x4.hpp>

#include "Commands/GraphicCommands.h"
#include "Rendering/Buffers/FrameBuffer.h"
#include "Rendering/Buffers/ObjectBuffer.h"
#include "Rendering/Buffers/LineBuffer.h"
#include "Rendering/Buffers/G_Buffer.h"
#include "Rendering/Buffers/ConstantBuffer.h"  

#include <memory>

using namespace Microsoft::WRL;


class GraphicsEngine
{
	friend class GraphicCommand; 

private:
	FrameBuffer myFrameBuffer;
	ObjectBuffer myObjectBuffer;
	LineBuffer myLineBuffer;
	LightBuffer myLightBuffer;
	G_Buffer myG_Buffer;

	std::vector<std::unique_ptr<GraphicCommand>> deferredCommandList;

	SIZE myWindowSize{0,0};	
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
	ComPtr<ID3D11SamplerState> myBRDFSampleState;
	// We're a container singleton, no instancing this outside the class.
	GraphicsEngine() = default;

public:

	static GraphicsEngine& Get() { static GraphicsEngine myInstance; return myInstance; }

	/**
	 * Initializes the Graphics Engine with the specified settings.
	 * @param windowHandle The window that will contain this Graphics Engine.
	 * @param enableDeviceDebug If DirectX should write debug output in the Output.
	 */
	bool Initialize(HWND windowHandle,bool enableDeviceDebug);

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
	void RenderFrame(float aDeltaTime,double aTotalTime);
	CU::Vector4<float>& GetBackgroundColor() { return myBackgroundColor; }

	template<typename T,typename...Types>
	void AddCommand(Types... args);

	[[nodiscard]] HWND FORCEINLINE GetWindowHandle() const { return myWindowHandle; }
	[[nodiscard]] SIZE FORCEINLINE GetWindowSize() const { return myWindowSize; }

	FORCEINLINE std::shared_ptr<Shader> GetDefaultVSShader() const { return defaultVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDefaultPSShader() const { return defaultPS; }

	FORCEINLINE std::shared_ptr<Shader> GetDebugLineVS() const { return debugLineVS; }
	FORCEINLINE std::shared_ptr<Shader> GetDebugLinePS() const { return debugLinePS; }


	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture() const { return defaultTexture; }
	FORCEINLINE std::shared_ptr<Material> GetDefaultMaterial() const { return defaultMaterial; }
	FORCEINLINE std::shared_ptr<TextureHolder> GetDefaultTexture(eTextureType type) const
	{
		switch(type)
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

template<typename T,typename...Types>
FORCEINLINE void GraphicsEngine::AddCommand(Types... args)
{
	deferredCommandList.push_back(std::make_unique<T>(args...));
}
