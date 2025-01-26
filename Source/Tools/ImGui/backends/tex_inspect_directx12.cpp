#include "tex_inspect_directx12.h"
#include "imgui.h"
#include "imgui_tex_inspect_internal.h"
#include <d3d12.h>
#include <Engine/AssetManager/AssetManager.h>
#include <Engine/AssetManager/Objects/BaseAssets/ShipyardShader.h>
#include "imgui_impl_dx12.cpp"

namespace ImGuiTexInspect {

    /* Constant buffer used in pixel shader. Size must be multiple of 16 bytes.
     * Layout must match the layout in the pixel shader. */
    struct PIXEL_CONSTANT_BUFFER {
        float ColorTransform[16];
        float ColorOffset[4];
        float Grid[4];
        float GridWidth[2];
        float PremultiplyAlpha;
        float DisableFinalAlpha;
        float BackgroundColor[3];
        bool  forceNearestSampling;
        float TextureSize[2];
        float padding[2];
    };

    struct ImGuiTexInspect_ImplDX12_Data {
        ID3D12Device* pd3dDevice = NULL;
        ID3D12RootSignature* pRootSignature;
        ID3D12PipelineState* pPipelineState;
        DXGI_FORMAT                 RTVFormat;
    };

    static ImGuiTexInspect_ImplDX12_Data GImplData;


    bool ImplDX12_Init(ID3D12Device* device,int num_frames_in_flight) {
        assert(GImplData.pd3dDevice == NULL);
        GImplData.pd3dDevice = device;
        GImplData.pd3dDevice->AddRef();

        

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
        memset(&psoDesc,0,sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
        psoDesc.NodeMask = 1;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.pRootSignature = GImplData.pRootSignature;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = GImplData.RTVFormat;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

        ID3DBlob* vertexShaderBlob;
        ID3DBlob* pixelShaderBlob;

        // Create the vertex shader
        {
            static const char* vertexShader =
                "cbuffer vertexBuffer : register(b0) \
            {\
              float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
              float2 pos : POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
              float4 pos : SV_POSITION;\
              float4 col : COLOR0;\
              float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
              PS_INPUT output;\
              output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
              output.col = input.col;\
              output.uv  = input.uv;\
              return output;\
            }";

            if(FAILED(D3DCompile(vertexShader,strlen(vertexShader),nullptr,nullptr,nullptr,"main","vs_5_0",0,0,&vertexShaderBlob,nullptr)))
                return false; // NB: Pass ID3DBlob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            psoDesc.VS = {vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize()};

            // Create the input layout
            static D3D12_INPUT_ELEMENT_DESC local_layout[] =
            {
                { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)offsetof(ImDrawVert, uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)offsetof(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            };
            psoDesc.InputLayout = {local_layout, 3};
        }

        // Create the pixel shader
        {
            std::shared_ptr<ShipyardShader> psShader;
            ENGINE_RESOURCES.ForceLoadAsset<ShipyardShader>("Shaders/ImGuiTexInspect.cso",psShader);
            psoDesc.PS = {psShader->GetBufferPtr(), psShader->GetBlobSize()};
        }

        // Create the blending setup
        {
            D3D12_BLEND_DESC& desc = psoDesc.BlendState;
            desc.AlphaToCoverageEnable = false;
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        // Create the rasterizer state
        {
            D3D12_RASTERIZER_DESC& desc = psoDesc.RasterizerState;
            desc.FillMode = D3D12_FILL_MODE_SOLID;
            desc.CullMode = D3D12_CULL_MODE_NONE;
            desc.FrontCounterClockwise = FALSE;
            desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
            desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
            desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            desc.DepthClipEnable = true;
            desc.MultisampleEnable = FALSE;
            desc.AntialiasedLineEnable = FALSE;
            desc.ForcedSampleCount = 0;
            desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        }

        // Create depth-stencil State
        {
            D3D12_DEPTH_STENCIL_DESC& desc = psoDesc.DepthStencilState;
            desc.DepthEnable = false;
            desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            desc.StencilEnable = false;
            desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            desc.BackFace = desc.FrontFace;
        }

        HRESULT result_pipeline_state = GImplData.pd3dDevice->CreateGraphicsPipelineState(&psoDesc,IID_PPV_ARGS(&GImplData.pPipelineState));
        vertexShaderBlob->Release();
        pixelShaderBlob->Release();
        if(result_pipeline_state != S_OK)
            return false;

        return true;
    }

    void ImplDX12_Shutdown() {
        if(GImplData.pd3dDevice)
            GImplData.pd3dDevice->Release();  

        GImplData.pd3dDevice = NULL;
    }

    void GiveNotInitializedWarning() {
        static bool warningGiven = false;
        if(!warningGiven) {
            fprintf(stderr,"ERROR: ImGuiTexInspect backend not initialized\n");
            warningGiven = true;
        }
    }

    struct DX11FormatDesc {
        ImGuiDataType_ type;
        int channelCount;

        int GetComponentSize() {
            switch(type) {
            case ImGuiDataType_S8:     return 1;
            case ImGuiDataType_U8:     return 1;
            case ImGuiDataType_S16:    return 2;
            case ImGuiDataType_U16:    return 2;
            case ImGuiDataType_S32:    return 4;
            case ImGuiDataType_U32:    return 4;
            case ImGuiDataType_S64:    return 8;
            case ImGuiDataType_U64:    return 8;
            case ImGuiDataType_Float:  return 4;
            case ImGuiDataType_Double: return 8;
            default:
                // Shouldn't get here
                assert(false);
                return 0;
            }
        }
    };

    bool DecodeDXGIFormat(DXGI_FORMAT format,DX11FormatDesc* desc) {
        switch(format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
            desc->type = ImGuiDataType_U8;
            desc->channelCount = 4;
            return true;

        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
            desc->type = ImGuiDataType_U8;
            desc->channelCount = 2;
            return true;

        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
            desc->type = ImGuiDataType_U8;
            desc->channelCount = 1;
            return true;

        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            desc->type = ImGuiDataType_Float;
            desc->channelCount = 4;
            return true;

        case DXGI_FORMAT_R32G32B32_FLOAT:
            desc->type = ImGuiDataType_Float;
            desc->channelCount = 3;
            return true;

        case DXGI_FORMAT_R32G32_FLOAT:
            desc->type = ImGuiDataType_Float;
            desc->channelCount = 2;
            return true;

        case DXGI_FORMAT_R32_FLOAT:
            desc->type = ImGuiDataType_Float;
            desc->channelCount = 1;
            return true;

        default:
            return false;
        }
    }

    //-------------------------------------------------------------------------
    // [SECTION] BackEnd functions declared in imgui_tex_inspect_internal.h
    //-------------------------------------------------------------------------

    void BackEnd_SetShader(const ImDrawList* cmdList,const ImDrawCmd* cmd,const Inspector* inspector) {
        /*if(GImplData.pPixelShader == NULL) {
            GiveNotInitializedWarning();
            return;
        }
        ImGui_ImplDX12_ViewportData* vd = (ImGui_ImplDX12_ViewportData*)cmdList->OwnerViewport->RendererUserData;
        cmd->SetPipelineState(bd->pPipelineState);
        cmdList->SetPipelineState(bd->pPipelineState);
        cmdList->SetGraphicsRootSignature(bd->pRootSignature);
        */

        //const ShaderOptions* shaderOptions = &inspector->CachedShaderOptions;

        //// Map the pixel shader constant buffer and fill values
        //{
        //    D3D11_MAPPED_SUBRESOURCE mapped_resource;
        //    if(GImplData.pd3dDeviceContext->Map(GImplData.pPixelConstantBuffer,0,D3D11_MAP_WRITE_DISCARD,0,&mapped_resource) != S_OK)
        //        return;


        //    // Transfer shader options from shaderOptions to our backend specific pixel shader constant buffer
        //    PIXEL_CONSTANT_BUFFER* constant_buffer = (PIXEL_CONSTANT_BUFFER*)mapped_resource.pData;

        //    memcpy(constant_buffer->ColorTransform,shaderOptions->ColorTransform,sizeof(shaderOptions->ColorTransform));
        //    memcpy(constant_buffer->ColorOffset,shaderOptions->ColorOffset,sizeof(shaderOptions->ColorOffset));
        //    memcpy(constant_buffer->Grid,&shaderOptions->GridColor,sizeof(shaderOptions->GridColor));
        //    memcpy(constant_buffer->GridWidth,&shaderOptions->GridWidth,sizeof(shaderOptions->GridWidth));
        //    memcpy(constant_buffer->BackgroundColor,&shaderOptions->BackgroundColor,sizeof(shaderOptions->BackgroundColor));
        //    memcpy(constant_buffer->TextureSize,&inspector->TextureSize,sizeof(inspector->TextureSize));

        //    constant_buffer->PremultiplyAlpha = shaderOptions->PremultiplyAlpha;
        //    constant_buffer->DisableFinalAlpha = shaderOptions->DisableFinalAlpha;
        //    constant_buffer->forceNearestSampling = shaderOptions->ForceNearestSampling;

        //    GImplData.pd3dDeviceContext->Unmap(GImplData.pPixelConstantBuffer,0);
        //}

        //// Activate shader and buffer
        //GImplData.pd3dDeviceContext->PSSetShader(GImplData.pPixelShader,NULL,0);
        //GImplData.pd3dDeviceContext->PSSetConstantBuffers(0,1,&GImplData.pPixelConstantBuffer);
    }

    bool BackEnd_GetData(Inspector* inspector,ImTextureID texture,int /*x*/,int /*y*/,int /*width*/,int /*height*/,BufferDesc* bufferDesc) {
        if(GImplData.pd3dDevice == NULL) {
            GiveNotInitializedWarning();
            return false;
        }
        //ID3D11Texture2D* pTexture = NULL;

        //// Get a pointer to the texture
        //{
        //    ID3D11ShaderResourceView* pTextureView = (ID3D11ShaderResourceView*)texture;
        //    ID3D11Resource* pResource;
        //    pTextureView->GetResource(&pResource);

        //    if(pResource == NULL)
        //        return false;

        //    pResource->QueryInterface<ID3D11Texture2D>(&pTexture);

        //    if(pTexture == NULL) {
        //        pResource->Release();
        //        return false;
        //    }
        //    pResource->Release();
        //}

        //int texWidth = (int)inspector->TextureSize.x;
        //int texHeight = (int)inspector->TextureSize.y;

        //ID3D11Texture2D* pStagingTexture = nullptr;
        //DX11FormatDesc formatDesc;

        //// Create a CPU accessible texture to copy src texture into
        //{
        //    D3D11_TEXTURE2D_DESC texDesc;
        //    pTexture->GetDesc(&texDesc);

        //    if(!DecodeDXGIFormat(texDesc.Format,&formatDesc)) {
        //        // Not a supoorted format
        //        pTexture->Release();
        //        return false;
        //    }

        //    // We'll keep all settings as is except for these:
        //    texDesc.Usage = D3D11_USAGE_STAGING;
        //    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        //    texDesc.BindFlags = 0;
        //    texDesc.MiscFlags = 0;

        //    /* Yes we could be keeping this staging texture around rather than creating it
        //     * every time.  But in profiling it was apparent that the cost of creating the
        //     * texture is not significant compared to copying the data.  So it's not worth the
        //     * complexity of tracking and releasing a texture for each Inspector.
        //     */
        //    HRESULT hr = GImplData.pd3dDevice->CreateTexture2D(&texDesc,NULL,&pStagingTexture);

        //    if(FAILED(hr)) {
        //        pTexture->Release();
        //        return false;
        //    }
        //}

        //// Oopy texture data to CPU accessible texture
        //GImplData.pd3dDeviceContext->CopyResource(pStagingTexture,pTexture);

        //D3D11_MAPPED_SUBRESOURCE resourceDesc = {};
        //HRESULT hr = GImplData.pd3dDeviceContext->Map(pStagingTexture,0,D3D11_MAP_READ,0,&resourceDesc);
        //bool success = false;
        //if(SUCCEEDED(hr)) {
        //    int componentSize = formatDesc.GetComponentSize();
        //    int channelCount = formatDesc.channelCount;

        //    void* copyDst = NULL;
        //    if(formatDesc.type == ImGuiDataType_Float) {
        //        bufferDesc->Data_float =
        //            (float*)ImGuiTexInspect::GetBuffer(inspector,(size_t)texWidth * texHeight * channelCount * componentSize);
        //        bufferDesc->Data_uint8_t = NULL;
        //        copyDst = bufferDesc->Data_float;
        //    } else // ImGuiDataType_U8 is only other supported type at the moment
        //    {
        //        bufferDesc->Data_uint8_t = ImGuiTexInspect::GetBuffer(inspector,(size_t)texWidth * texHeight * channelCount * componentSize);
        //        bufferDesc->Data_float = NULL;
        //        copyDst = bufferDesc->Data_uint8_t;
        //    }

        //    if(copyDst) {
        //        int const bytesPerPixel = componentSize * channelCount;
        //        for(int i = 0; i < texHeight; ++i) {
        //            /* TODO : An obvious optimization would be to return a pointer
        //             * directly into the DirectX ResourceDesc data.  We would need
        //             * another callback to know when to unmap it.
        //             */
        //            memcpy((byte*)copyDst + texWidth * bytesPerPixel * i,(byte*)resourceDesc.pData + resourceDesc.RowPitch * i,
        //                (size_t)texWidth * bytesPerPixel);
        //        }
        //        bufferDesc->BufferByteSize = (size_t)texWidth * texHeight * bytesPerPixel;
        //        bufferDesc->Red = 0;
        //        bufferDesc->Green = (ImU8)ImMin(1,channelCount - 1);
        //        bufferDesc->Blue = (ImU8)ImMin(2,channelCount - 1);
        //        bufferDesc->Alpha = (ImU8)ImMin(3,channelCount - 1);
        //        bufferDesc->ChannelCount = (ImU8)channelCount;

        //        bufferDesc->LineStride = (int)inspector->TextureSize.x * channelCount;
        //        bufferDesc->Stride = channelCount;
        //        bufferDesc->StartX = 0;
        //        bufferDesc->StartY = 0;
        //        bufferDesc->Width = texWidth;
        //        bufferDesc->Height = texHeight;

        //        success = true;
        //    }
        //    GImplData.pd3dDeviceContext->Unmap(pStagingTexture,0);
        //}

        //pTexture->Release();

        //pStagingTexture->Release();

        return true;
    }
} // namespace ImGuiTexInspect
