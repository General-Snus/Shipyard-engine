#include "DirectXHeader.pch.h"

#include "../Helpers.h"
#include "Graphics/GPU.h"

namespace Helpers
{

void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            hr = GPUInstance.m_Device->GetDeviceRemovedReason();
        }

        const _com_error err(hr);
        const std::wstring errMsg = (err.ErrorMessage());
        Logger.Err(Helpers::string_cast<std::string>(errMsg));
        throw std::exception(Helpers::string_cast<std::string>(errMsg).c_str());
    }
}
} // namespace Helpers
