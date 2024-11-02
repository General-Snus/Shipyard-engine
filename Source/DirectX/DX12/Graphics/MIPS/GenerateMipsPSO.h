#pragma once

/*
 *  Copyright(c) 2018 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

 /**
  *  @file GenerateMipsPSO.h
  *  @date October 24, 2018
  *  @author Jeremiah van Oosten
  *
  *  @brief Pipeline state object for generating mip maps.
  */

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

#include "DirectX/DX12/Graphics/GpuResource.h"
#include "DirectX/DX12/Graphics/PSO.h"
#include "DirectX/DX12/Graphics/RootSignature.h"

//  //alignas(16)
//struct  GenerateMipsCB
//{
//	uint32_t SrcMipLevel;           // Texture level of source mip
//	uint32_t NumMipLevels;          // Number of OutMips to write: [1-4]
//	uint32_t SrcDimension;          // Width and height of the source texture are even or odd.
//	uint32_t IsSRGB;                // Must apply gamma correction to sRGB textures.
//	DirectX::XMFLOAT2 TexelSize;    // 1.0 / OutMip1.Dimensions
//};
//
//class GenerateMipsPSO : public PSO
//{
//public:
//	GenerateMipsPSO() = default;
//
//	void Init(const ComPtr<ID3D12Device2>& dev) override;
//	/*
//	D3D12_CPU_DESCRIPTOR_HANDLE GetDefaultUAV() const
//	{
//		return m_DefaultUAV.GetHandle();
//	}*/
//
//private:
//	// Default (no resource) UAV's to pad the unused UAV descriptors.
//	// If generating less than 4 mip map levels, the unused mip maps
//	// need to be padded with default UAVs (to keep the DX12 runtime happy).
//	UAVResource m_DefaultUAV;
//};