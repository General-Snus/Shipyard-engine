#include "DirectX/DirectXHeader.pch.h" 
#include "../Helpers.h"
#include "DirectX/Shipyard/GPU.h"

namespace Helpers
{
	
	void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			if (hr == DXGI_ERROR_DEVICE_REMOVED)
			{
				hr = GPU::m_Device->GetDeviceRemovedReason();
			}

			_com_error err(hr);
			std::wstring errMsg = std::to_wstring(*err.ErrorMessage());
			Logger::Err(Helpers::string_cast<std::string>(errMsg));
			throw std::exception(Helpers::string_cast<std::string>(errMsg).c_str());
		}
	}
}