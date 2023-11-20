#pragma once
#define WIN32_LEAN_AND_MEAN
#include <thread>

#include "Windows.h"

LRESULT CALLBACK SplashWinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

class SplashWindow
{
	HWND myHandle, myLayeredHandle;
	HBITMAP bmp;
	HINSTANCE myHInstance;

	std::thread myThread;

	bool bIsRunning = true;

	void MessagePump();

	void CreateAndShowWindow();

public:

	HBITMAP GetBitmap() const { return bmp; }

	void Init(HINSTANCE hInstance);
	void Close();

	~SplashWindow();
};

