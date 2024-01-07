#pragma once
#include <stdexcept>
#include <string>
#include <array>
#include <memory>
#include <wrl.h>
#include <d3d11.h>

using namespace Microsoft::WRL;

struct ID3D11Resource;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct D3D11_VIEWPORT;

/**
 * \brief Represents raw texture data in some format.
 */
class Texture
{
	friend class RHI;
	friend class TextureHolder;

protected:

	// The name of this texture, for easy ID.
	std::wstring myName;

	// The actual data container for this texture.
	ComPtr<ID3D11Resource> myTexture = nullptr;

	// Different views. Which are valid and usable
	// depends on the Flags used when this texture
	// was created.
	ComPtr<ID3D11ShaderResourceView> mySRV = nullptr;

	// We can be either a Render Target or a Depth Stencil.
	ComPtr<ID3D11RenderTargetView> myRTV = nullptr;
	ComPtr<ID3D11DepthStencilView> myDSV = nullptr;

	D3D11_VIEWPORT myViewport{};

	UINT myBindFlags{};
	UINT myUsageFlags{};
	UINT myAccessFlags{};

public:
	Texture() = default;
	~Texture() = default;

	FORCEINLINE const std::wstring& GetName() const { return myName; }
	FORCEINLINE UINT GetBindFlags() const { return myBindFlags; }
	FORCEINLINE UINT GetUsageFlags() const { return myUsageFlags; }
	FORCEINLINE UINT GetAccessFlags() const { return myAccessFlags; }
	FORCEINLINE bool IsValid() const { return myTexture != nullptr; }
	FORCEINLINE ID3D11ShaderResourceView* GetSRV() const { return mySRV.Get(); }
};