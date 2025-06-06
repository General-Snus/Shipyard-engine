#include "../Window.h"
#include <Windows.h>

#include <combaseapi.h>
#include <shellapi.h>
#include <string> 
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include "Tools/Logging/Logging.h"

void Window::Init(const WinInitSettings& init)
{
	#if false // Allocate console  
	FILE* consoleOut;
	AllocConsole();
	freopen_s(&consoleOut,"CONOUT$","w",stdout);
	freopen_s(&consoleOut,"CONOUT$","w",stderr);
	setvbuf(consoleOut,nullptr,_IONBF,1024);

	HWND consoleWindow = GetConsoleWindow();
	RECT consoleSize;
	GetWindowRect(consoleWindow,&consoleSize);
	MoveWindow(consoleWindow,consoleSize.left,consoleSize.top,1280,720,true);

	MoveConsoleToOtherMonitor();
	#endif

	moduleHandler = init.hInstance;

	const HRESULT hr = CoInitializeEx(nullptr,COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
		LOGGER.Err("Failed to initialize COM");
		return;
	}

	constexpr auto windowClassName = L"ShipyardEditorWindow";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hCursor = LoadCursor(nullptr,IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	SetProcessDPIAware();
	// Then we use the class to create our window
	windowHandler = CreateWindow(windowClassName,                // Classname
								 init.windowTitle.c_str(),       // Window Title
								 WS_OVERLAPPEDWINDOW | WS_POPUP, // Flags
								 (GetSystemMetrics(SM_CXSCREEN) - init.windowSize.x) / 2,
								 (GetSystemMetrics(SM_CYSCREEN) - init.windowSize.y) / 2,init.windowSize.x,
								 init.windowSize.y,nullptr,nullptr,windowClass.hInstance,nullptr);

	if(windowHandler == nullptr)
	{
		LOGGER.Err("Failed to create window");
		return;
	}

	ShowWindow(windowHandler,SW_SHOWDEFAULT);
	UpdateWindow(windowHandler);
	DragAcceptFiles(windowHandler,TRUE);
}

unsigned int Window::MonitorWidth() const
{
	return MonitorResolution().x;
}

unsigned int Window::MonitorHeight() const
{
	return MonitorResolution().y;
}

Vector2ui Window::MonitorResolution() const
{
	return Vector2ui(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}

bool Window::SetWindowsTitle(const std::string& titleName) const {
	assert(windowHandler != nullptr);
	return SetWindowTextA(windowHandler,titleName.c_str());
}
std::string Window::GetWindowsTitle() const
{ 
	auto cTxtLen = GetWindowTextLengthA(windowHandler);
	std::string str;
	str.reserve(cTxtLen);
	GetWindowTextA(windowHandler,str.data(),cTxtLen + 1);
	return str;
}

bool Window::Update()
{
	MSG msg{};
	while(PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(msg.message == WM_QUIT)
		{
			return false;
		}
	}
	return true;
}

LRESULT CALLBACK Window::WinProc(_In_ HWND hWnd,_In_ UINT uMsg,_In_ WPARAM wParam,_In_ LPARAM lParam)
{
	if(WindowInstance.callback)
	{
		MSG msg{};
		msg.hwnd = hWnd;
		msg.message = uMsg;
		msg.wParam = wParam;
		msg.lParam = lParam;
		WindowInstance.callback(&WindowInstance,msg);
	}

	switch(uMsg)
	{
	case WM_DESTROY:
	{
		DragAcceptFiles(hWnd,FALSE);
		PostQuitMessage(0);
		return 0;
	}
	default:
	break;
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void Window::SetCallbackFunction(const std::function<void(Window* window,const MSG& msg)>& aCallback)
{
	callback = aCallback;
}

void Window::Destroy()
{
	callback = nullptr;
	CoUninitialize();

	if(windowHandler)
	{
		DestroyWindow(windowHandler);
	}
}

void Window::MoveConsoleToOtherMonitor()
{
	const HWND     consoleWindow = GetConsoleWindow();
	const Vector2i consoleSize = {1280,720};
	const int      monitorCount = GetSystemMetrics(SM_CMONITORS);
	if(monitorCount > 1)
	{
		RECT virtualSize;
		virtualSize.right = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		virtualSize.bottom = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		virtualSize.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		virtualSize.top = GetSystemMetrics(SM_YVIRTUALSCREEN);

		RECT desktopSize;
		GetWindowRect(GetDesktopWindow(),&desktopSize);

		Vector2i consolePos;
		if(virtualSize.left < 0 && desktopSize.right < virtualSize.right) // Secondary monitor to the left
		{
			consolePos.x = desktopSize.left - consoleSize.x;
			consolePos.y = desktopSize.top;
		} else if(desktopSize.right < virtualSize.right) // Secondary monitor to the right
		{
			consolePos.x = virtualSize.right - desktopSize.right;
			consolePos.y = desktopSize.top;
		} else if(virtualSize.top < 0) // Secondary monitor on top
		{
			// I am too lazy too account for anything else than the taskbar being on
			// the bottom of the screen
			RECT taskbar;
			GetWindowRect(FindWindow(L"Shell_traywnd",nullptr),&taskbar);
			const int taskbarHeight = taskbar.bottom - taskbar.top;
			consolePos.x = desktopSize.left;
			consolePos.y = desktopSize.top - consoleSize.y - taskbarHeight;
		} else // Secondary monitor below
		{
			consolePos.x = desktopSize.left;
			consolePos.y = virtualSize.bottom - desktopSize.bottom;
		}

		SetWindowPos(consoleWindow,HWND_TOP,consolePos.x,consolePos.y,consoleSize.x,consoleSize.y,0);
	} else
	{
		RECT consolePos;
		GetWindowRect(consoleWindow,&consolePos);
		MoveWindow(consoleWindow,consolePos.left,consolePos.top,consoleSize.x,consoleSize.y,true);
	}
}

unsigned int Window::Width() const
{
	RECT rect{};
	GetClientRect(windowHandler,&rect);
	return static_cast<unsigned int>(rect.right - rect.left);
}

unsigned int Window::Height() const
{
	RECT rect{};
	GetClientRect(windowHandler,&rect);
	return static_cast<unsigned int>(rect.bottom - rect.top);
}

Vector2ui Window::Resolution() const
{
	RECT rect{};
	GetClientRect(windowHandler,&rect);
	return Vector2ui(rect.right - rect.left,rect.bottom - rect.top);
}
