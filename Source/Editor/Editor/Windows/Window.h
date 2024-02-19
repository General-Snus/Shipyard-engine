#pragma once
#include <functional>
#include "Windows.h" 
struct WinInitSettings
{
	LPCWSTR windowTitle;
	SIZE windowSize;
	HINSTANCE hInstance;
};



class Window
{
public:

	static void Init(const WinInitSettings& init);
	static bool Update();
	static LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	static void SetCallbackFunction(const std::function<void(MSG const& msg)>& aCallback);
	static void Destroy();
	static void MoveConsoleToOtherMonitor();

	static unsigned int Width();
	static unsigned int Height();


	inline static HINSTANCE moduleHandler;
	inline static HWND windowHandler;
	//Cred goes to adira guy on reddit for wonderfull code
	inline static std::function<void(const MSG& msg)> callback = nullptr;
};

