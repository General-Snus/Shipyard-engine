#pragma once

#include <array>
#include "d3d11.h"
#include "d3d11_1.h"
#include "d3d11shader.h"
#include "d3dcompiler.h"

#include <memory>
#include <string>
#include <vector>

#include "ShaderInfo.h"

#pragma comment(lib, "d3dcompiler.lib")

// This is for the ComPtrs.
// Don't mistake it for the ATL CComPtr!
#include <unordered_map>
#include <wrl.h>
class Shader;
using namespace Microsoft::WRL;

//struct Vertex;
class Texture;
struct ConstantBufferBase;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3DUserDefinedAnnotation;
struct ID3D11ShaderReflection;

typedef
enum PIPELINE_STAGE
{
	PIPELINE_STAGE_INPUTASSEMBLER = 0x1L,
	PIPELINE_STAGE_VERTEX_SHADER = 0x2L,
	PIPELINE_STAGE_GEOMETERY_SHADER = 0x4L,
	PIPELINE_STAGE_RASTERIZER = 0x8L,
	PIPELINE_STAGE_PIXEL_SHADER = 0x10L,
	PIPELINE_STAGE_OUTPUT_MERGER = 0x20L,
} 	PIPELINE_STAGE;

enum RasterizerState
{
	RS_Default,
	RS_CullNone,
	RS_Wireframe,
	RS_COUNT
};

enum DepthState
{
	DS_Default,
	DS_LessEqual,
	DS_COUNT
};

/**
 * This is the Render Hardware Interface that communicates with DirectX 11
 */
class RHI
{
public:

	struct DeviceSize
	{
		unsigned Width;
		unsigned Height;
	};

	/**
	 * Initializes the DirectX 11 Render Hardware Interface.
	 * @param aWindowHandle The Window in which we shall render things.
	 * @param enableDeviceDebug If we want DirectX to print debugging information in the Visual Studio Output Window. Very handy.
	 * @param outBackBuffer The device Back Buffer we want to draw everything on. This essentially represents the screen.
	 * @param outDepthBuffer The device Depth Buffer. This is used as a 2D representation of 3D space, to determine if an object is in front of or behind another.
	 * @returns True if initialization was successful.
	 */
	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,Texture* outBackBuffer,Texture*
		outDepthBuffer);
	static bool UnInitialize();

	/**
	 * Instructs the Swap Chain to change buffer, displaying the back buffer on screen and giving us a different back buffer to draw on next.
	 */
	static void Present(unsigned aSyncInterval = 0);

	static const DeviceSize& GetDeviceSize() { return myDeviceSize; }

	/**
	 * Creates a Vertex Buffer object in the Graphics Card memory.
	 * @param outVxBuffer The resulting Vertex Buffer pointer.
	 * @param aVertexList A list of vertices that will be used to fill the buffer in the graphics card memory.
	 */
	template<typename VertexType>
	static bool CreateVertexBuffer(ComPtr<ID3D11Buffer>& outVxBuffer,const std::vector<VertexType>& aVertexList);

	/**
	 * Creates a dynamically resizeable Vertex Buffer object in the Graphics Card memory.
	 * @param outVxBuffer the resulting Vertex Buffer pointer.
	 * @param aMaxNumberOfVertices The maximum possible size of this vertex buffer.
	 * @param aVertexTypeSize The size of a single vertex.
	 */
	static bool CreateDynamicVertexBuffer(ComPtr<ID3D11Buffer>& outVxBuffer,size_t aMaxNumberOfVertices,size_t aVertexTypeSize);

	/**
	 * Creates an Index Buffer object in the Graphics Card memory.
	 * @param outIxBuffer The resulting Index Buffer pointer.
	 * @param anIndexList A list of indices that will be used to fill the buffer in the graphics card memory.
	 */
	static bool CreateIndexBuffer(ComPtr<ID3D11Buffer>& outIxBuffer,const std::vector<unsigned>& anIndexList);

	/**
	 * Creates a dynamically resizeable Index Buffer object in the Graphics Card memoy.
	 * @param outIxBuffer The resulting Index Buffer pointer.
	 * @param aMaxNumberOfIndices The maximum possible size of this index buffer.
	 */
	static bool CreateDynamicIndexBuffer(ComPtr<ID3D11Buffer>& outIxBuffer,size_t aMaxNumberOfIndices);

	/**
	 * Creates a Constant Buffer object in the Graphics Card memory.
	 * @param outCBuffer The resulting Constant Buffer pointer.
	 * @param aSize The size of the constant buffer object to create. Should probably be a sizeof some struct. Must be a multiple of 16 bytes, and max size is 65536 bytes, or 64kB!
	 */
	static bool CreateConstantBuffer(ComPtr<ID3D11Buffer>& outCBuffer,size_t aSize);

	/**
	 * Updates the Constant Buffer object in Graphics Card memory with the provided data.
	 * @param aBuffer A pointer to the Constant Buffer.
	 */
	static void UpdateConstantBufferData(const ConstantBufferBase& aBuffer);

	/**
	 * Creates a method for sampling textures based on the provided description.
	 * @param outSamplerState The resulting Sampler State pointer.
	 * @param aDescription A Direct3D 11 Sampler Description detailing how this sampler state should operate.
	 */
	static bool CreateSamplerState(ComPtr<ID3D11SamplerState>& outSamplerState,const D3D11_SAMPLER_DESC& aDescription);

	/**
	 * Instructs the current Pixel Shader which Sampler State to use on the specified slot.
	 * @param aSamplerState The sampler state pointer to use.
	 * @param aSlot The slot to set the sampler state on.
	 */
	static void SetSamplerState(const ComPtr<ID3D11SamplerState>& aSamplerState,unsigned aSlot);

	/**
	 * Creates a Blend State with the provided description.
	 * @param outBlendState The resulting Blend State pointer.
	 * @param aDescription A Direct3D 11 Blend Description detailing how this blend state should operate.
	 */
	static bool CreateBlendState(ComPtr<ID3D11BlendState>& outBlendState,const D3D11_BLEND_DESC& aDescription);

	/**
	 * Instructs the Output Merger on how to assemble rendered pixels.
	 * @param aBlendState The Blend State to use.
	 * @param aBlendFactor How we should blend RGBA pixels, depends on the Blend State settings.
	 * @param aSamplerMask A mask controlling how a multisample render target should be written.
	 */
	static void SetBlendState(const ComPtr<ID3D11BlendState>& aBlendState,const std::array<float,4>& aBlendFactor = {0, 0, 0, 0},unsigned aSamplerMask = 0xffffffff);

	/**
	 * Attempts to load a Vertex Shader from the specified file name, then create a Vertex Shader object and an Input Layout object on the Graphics Card.
	 * @param outVxShader The resulting Vertex Shader pointer.
	 * @param outInputLayout The resulting Input Layout pointer.
	 * @param aFileName A path to a CSO file, Compiled Shader Object, from which to load the Vertex Shader.
	 * @param anInputLayoutDesc A list of Input Layout Elements that describe a Vertex.
	 */
	static bool LoadVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader>& outVxShader,
		ComPtr<ID3D11InputLayout>& outInputLayout,
		const std::wstring& aFileName,const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc);

	/**
	 * Attempts to create a Vertex Shader and Input Layout from the specified shader bytecode.
	 * @param outVxShader The resulting Vertex Shader pointer.
	 * @param outInputLayout The resulting Input Layout pointer.
	 * @param anInputLayoutDesc A list of Input Layout Elements that describe a Vertex.
	 * @param someShaderData A stream of compiled shader data.
	 * @param aShaderDataSize The length of the compiled shader data.
	 */
	static bool CreateVertexShaderAndInputLayout(ComPtr<ID3D11VertexShader>& outVxShader,
		ComPtr<ID3D11InputLayout>& outInputLayout,const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc,const BYTE* someShaderData,size_t aShaderDataSize);

	static bool CreateInputLayout(ComPtr<ID3D11InputLayout>& outInputLayout,const std::vector<D3D11_INPUT_ELEMENT_DESC>& anInputLayoutDesc,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to create a Vertex Shader from the specified shader bytecode.
	 * @param outVxShader The resulting Vertex Shader pointer.
	 * @param someShaderData A stream of compiled shader data.
	 * @param aShaderDataSize The length of the compiled shader data.
	 */
	static bool CreateVertexShader(ComPtr<ID3D11VertexShader>& outVxShader,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to create a Pixel Shader from teh specified shader bytecode.
	 * @param outPxShader The resulting Pixel Shader pointer.
	 * @param someShaderData A stream of compiled shader data.
	 * @param aShaderDataSize The length of the compiled shader data.
	 */
	static bool CreatePixelShader(ComPtr<ID3D11PixelShader>& outPxShader,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to extract reflection information from the provided shader blob data.
	 * @param outShRefl The resulting shader reflection information.
	 * @param someShaderData The shader byte code to extract data from.
	 * @param aShaderDataSize Size of the shader byte code.
	 */
	static bool ReflectShader(ShaderInfo& outShRefl,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to create a Geometry Shader from teh specified shader bytecode.
	 * @param outGeoShader The resulting Geometry Shader pointer.
	 * @param someShaderData A stream of compiled shader data.
	 * @param aShaderDataSize The length of the compiled shader data.
	 */
	static bool CreateGeometryShader(ComPtr<ID3D11GeometryShader>& outGeoShader,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to load a Pixel Shader from the specified file name and create a Pixel Shader object on the Graphics Card.
	 * @param outPxShader The resulting Pixel Shader pointer.
	 * @param aFileName A path to a CSO file, Compiled Shader Object, from which to load the Pixel Shader.
	 */
	static bool LoadPixelShader(ComPtr<ID3D11PixelShader>& outPxShader,const std::wstring& aFileName);

	/**
	 * Attempts to load a Texture from the specified file name and create a Texture object on the Graphics Card.
	 * @param outTexture The resulting Texture pointer.
	 * @param aFileName A path to a DDS texture file, from which to load the Texture.
	 */
	static bool LoadTexture(Texture* outTexture,const std::wstring& aFileName);

	/***
	 * Attempts to load any shader type from the specified file name, and create a Shader object on the Graphics Card.
	 * @param outShader The resulting Shader pointer.
	 * @param aFileName A path to the compiled cso file, from which to load the shader.
	 */
	static bool LoadShader(Shader* outShader,const std::wstring& aFileName);

	/**
	 * Attempts to load any shader type from the provided data blob, and create a Shader object on the Graphics Card.
	 * @param outShader The resulting Shader pointer.
	 * @param aName A name to give the shader.
	 * @param someShaderData A stream of compiled shader data.
	 * @param aShaderDataSize The length of the compiled shader data.
	 */
	static bool LoadShaderFromMemory(Shader* outShader,const std::wstring& aName,const BYTE* someShaderData,size_t aShaderDataSize);

	/**
	 * Attempts to load a Texture from an in-memory data structure, and create a Texture object on the Graphics Card.
	 * @param outTexture The resulting Texture pointer.
	 * @param aName What to call this texture.
	 * @param someImageData The image byte data.
	 * @param anImageDataSize How long the image byte data is.
	 * @param aSRVDesc An optional Shader Resource View Desc to alter how the texture is interpreted when read.
	 */
	static bool LoadTextureFromMemory(Texture* outTexture,const std::wstring& aName,const BYTE* someImageData,size_t anImageDataSize,const
		D3D11_SHADER_RESOURCE_VIEW_DESC* aSRVDesc = nullptr);

	/**
	 * Configures the first stage of the Pipeline, the Input Assembler, with the specified data and settings. This prepares the Graphics Card for drawing something.
	 * @param aTopology The type of data we're wanting to draw, this is usually a Triangle List.
	 * @param aVxBuffer The Vertex Buffer that describes the data we want to draw, for example that of a Model.
	 * @param anIxBuffer The Index Buffer which describes the order Vertices in the Vertex Buffer should be used when drawing.
	 * @param aVertexStride The size of a single Vertex found in the Vertex Buffer.
	 * @param anInputLayout An Input Layout which describes how a single vertex looks in memory.
	 */
	static void ConfigureInputAssembler(unsigned aTopology,const ComPtr<ID3D11Buffer>& aVxBuffer,const ComPtr<ID3D11Buffer>& anIxBuffer,unsigned aVertexStride,const ComPtr<ID3D11InputLayout>& anInputLayout);

	/**
	 * Configures the 2nd stage of the Pipeline, the Vertex Shader Stage, to use the specified Vertex Shader program when drawing pixels.
	 * @param aVertexShader The Vertex Shader program to to use.
	 */
	static void SetVertexShader(const ComPtr<ID3D11VertexShader>& aVertexShader);
	static void SetVertexShader(const Shader* aVertexShader);

	/**
	 * Configures the 4th stage of the Pipeline, the Geometry Shader Stage, to use the specified Geometry Shader program when drawing pixels.
	 * @param aGeometryShader The Geometry Shader program to to use.
	 */
	static void SetGeometryShader(const ComPtr<ID3D11GeometryShader>& aGeometryShader);
	static void SetGeometryShader(const Shader* aGeometryShader);

	/**
	 * Configures the 6th stage of the Pipeline, the Pixel Shader Stage, to use the specified Pixel Shader program when drawing pixels.
	 * @param aPixelShader The Pixel Shader program to to use.
	 */
	static void SetPixelShader(const ComPtr<ID3D11PixelShader>& aPixelShader);
	static void SetPixelShader(const Shader* aPixelShader);

	/**
	 * Instruct the Rasterizer Stage to use the provided State when rasterizing geometry.
	 * @param aState The Rasterizer State to use.
	 */
	static void SetRasterizerState(const RasterizerState aState);

	/**
	 * Instruct the Output Merger to use the specified Depth State when composing the final image.
	 * @param aState The Depth State to use.
	 */
	static void SetDepthState(const DepthState aState);

	/**
	 * Binds a Constant Buffer to the specified slot, for the specified shader stages. Only affects those shader stages!
	 * @param aPipelineStages The pipeline stages to bind to, can be concatenated with a pipe | character.
	 * @param aSlot The slot to bind the buffer at for the provided stages.
	 * @param aBuffer The buffer to bind to this slot.
	 */
	static void SetConstantBuffer(UINT aPipelineStages,unsigned aSlot,const ConstantBufferBase& aBuffer);

	/**
	 * Instructs the Graphics Card to execute a Draw call using the data loaded in the pipeline with the states that are currently set.
	 * @param aCount The number of Vertices to draw.
	 */
	static void DrawIndexed(unsigned aCount);

	static void Draw(unsigned aCount);

	// This ought to be returning a Texture object which has a Resource, a SRV and a RTV. The latter two are created and populated on demand
	// depending on the BindFlags, i.e. if(someBindFlagS |= D3D11_BIND_SHADER_RESOURCE) create SRV.
	static bool CreateTexture(Texture* outTexture,const std::wstring& aName,
		size_t aWidth,size_t aHeight,UINT aFormat,D3D11_USAGE someUsageFlags = D3D11_USAGE_DEFAULT,
		UINT someBindFlags = D3D11_BIND_SHADER_RESOURCE,UINT someCpuAccessFlags = 0);

	static bool CreateTextureCube(Texture* outTexture,const std::wstring& aName,size_t aWidth,size_t aHeight,
		UINT aFormat,D3D11_USAGE someUsageFlags = D3D11_USAGE_DEFAULT,UINT someBindFlags = D3D11_BIND_SHADER_RESOURCE,
		UINT someCpuAccessFlags = 0);

	/**
	 * Binds a Texture to the specified slot, for the specified shader stages. Only affects those shader stages!
	 * @param aPipelineStages The pipeline stages to bind to, can be concatenated with a pipe | character.
	 * @param aSlot The slot to bind the texture on for the provided stages.
	 * @param aTexture The texture to bind to this slot.
	 */
	static void SetTextureResource(UINT aPipelineStages,unsigned aSlot,const Texture* aTexture);

	static void SetTextureResources(UINT aPipelineStages,unsigned aStartSlot,const std::vector<std::shared_ptr<Texture>>& aTextureList);

	/**
	 * Removes a Texture Resource from the specified slot, for the specified shader stages. Only affects those shader stages!
	 * @param aPipelineStages The pipeline stages to clear, can be concatenated with a pipe | character.
	 * @param aSlot The slot to clear from the provided stages.
	 */
	static void RemoveTextureResource(UINT aPipelineStages,unsigned aSlot);

	/**
	 * Sets the provided Texture as the active target in the Output Merger for all future Draw Calls.
	 * @param aTarget The texture we should draw on.
	 */
	static void SetRenderTarget(const Texture* aTarget,const Texture* aDepthStencil);

	static void SetRenderTargets(const std::vector<std::shared_ptr<Texture>>& aTargetList,const std::shared_ptr<Texture>& aDepthStencil);

	/**
	 * Sets multiple Textures as active targets in the Output Merger.
	 * @param aTargetList A list of Textures to draw on.
	 */
	static void SetRenderTargets(const std::vector<Texture*>& aTargetList,const Texture* aDepthStencil);

	static void SetDepthStencil(const Texture* aDepthStencil);

	static void CopyRegion(const Texture* aSource,const Texture* aDestination,unsigned X,unsigned Y,unsigned
		Width,unsigned Height);
	static std::array<BYTE,4> GetPixel(const Texture* aSource,unsigned X,unsigned Y);

	/**
	 * Removes the current Render Target from the Output Merger, leaving it empty.
	 */
	static void RemoveTarget();

	static void ClearRenderTarget(const Texture* aTexture,std::array<float,4> aClearColor = {0, 0, 0, 0});

	static void ClearDepthStencil(const Texture* aTexture);

	static void BeginEvent(const std::wstring& anEventName);
	static void EndEvent();

	static inline ComPtr<ID3D11Device> Device;
	static inline ComPtr<ID3D11DeviceContext> Context;

private:

	static inline ComPtr<IDXGISwapChain> mySwapChain;
	static inline ComPtr<ID3DUserDefinedAnnotation> myAnnotationObject;
	static inline DeviceSize myDeviceSize;

	static void ReportError(HRESULT aHresult,const std::wstring& aMessage = L"");
	static bool CreateVertexBufferInternal(ComPtr<ID3D11Buffer>& outVxBuffer,const uint8_t* aVertexDataPtr,size_t aNumVertices,size_t aVertexSize);
	static void ReflectShaderVariableMember(const std::wstring& aDomain,size_t& anOffset,ID3D11ShaderReflectionType* aMemberType,const D3D11_SHADER_TYPE_DESC*
		aMemberTypeDesc,ShaderInfo::ConstantBufferInfo& inoutBufferInfo);
	static void ReflectShaderVariable(const std::wstring& aDomain,size_t& anOffset,ID3D11ShaderReflectionVariable* aVariable,ShaderInfo::
		ConstantBufferInfo& inoutBufferInfo);

	static inline std::array<ComPtr<ID3D11RasterizerState>,RS_COUNT> myRasterizerStates;
	static inline std::array<ComPtr<ID3D11DepthStencilState>,DS_COUNT> myDepthStates;
};

template <typename VertexType>
bool RHI::CreateVertexBuffer(ComPtr<ID3D11Buffer>& outVxBuffer,const std::vector<VertexType>& aVertexList)
{
	const size_t vxSize = sizeof(VertexType);
	const size_t numVx = aVertexList.size();
	return CreateVertexBufferInternal(outVxBuffer,reinterpret_cast<const uint8_t*>(aVertexList.data()),numVx,vxSize);
}
