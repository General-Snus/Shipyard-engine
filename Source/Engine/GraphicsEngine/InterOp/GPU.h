#pragma once

class GPU
{
public:
	static bool Initialize(HWND aWindowHandle,bool enableDeviceDebug,Texture* outBackBuffer,Texture* outDepthBuffer);
	static bool UnInitialize();
	static void Present(unsigned aSyncInterval = 0);
};

