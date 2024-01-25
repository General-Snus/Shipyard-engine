#include "GraphicsEngine.pch.h"
#include <DirectX\directx\d3d12.h>
#include "GPU.h"

bool GPU::Initialize(HWND aWindowHandle,bool enableDeviceDebug,Texture* outBackBuffer,Texture* outDepthBuffer)
{
	aWindowHandle; enableDeviceDebug; outBackBuffer; outDepthBuffer;
	return false;
}

bool GPU::UnInitialize()
{
	return false;
}

void GPU::Present(unsigned aSyncInterval)
{
	aSyncInterval;
}
