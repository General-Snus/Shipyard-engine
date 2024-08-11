#pragma once
#include "Gpu_fwd.h"
#include <DirectX/Shipyard/GpuResource.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <unordered_map>

using namespace Microsoft::WRL;

class Texture : public GpuResource
{
    friend class GPU;
    friend class TextureHolder;
    friend class Material;

  public:
    explicit Texture();

    FORCEINLINE const std::string &GetName() const
    {
        return myName;
    }
    FORCEINLINE bool IsValid() const
    {
        return m_Resource != nullptr;
    }
    // The name of this texture, for easy ID.
    std::string myName;

    // Default state is render target
    bool AllocateTexture(const Vector2ui dimentions, const std::filesystem::path &name = "UnnamedTexture",
                         DXGI_FORMAT Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                         D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET |
                                                      D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS,
                         D3D12_RESOURCE_STATES targetResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET);

    void SetView(D3D12_SHADER_RESOURCE_VIEW_DESC view);
    void SetView(D3D12_UNORDERED_ACCESS_VIEW_DESC view);
    void SetView(ViewType view) override;

    uint32_t GetWidth() const
    {
        return static_cast<uint32_t>(m_Viewport.Width);
    }
    uint32_t GetHeight() const
    {
        return static_cast<uint32_t>(m_Viewport.Height);
    }

    Vector2ui GetResolution() const
    {
        return Vector2ui(GetWidth(), GetHeight());
    };

    // KEKW
    const D3D12_RECT &GetRect() const
    {
        return m_Rect;
    };
    const D3D12_VIEWPORT &GetViewPort() const
    {
        return m_Viewport;
    };

  protected:
    bool isCubeMap = false;
    Vector4f m_ClearColor = {0, 0, 0, 1};

    D3D12_VIEWPORT m_Viewport;
    D3D12_RECT m_Rect;
};
