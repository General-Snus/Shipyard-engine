#include "Window.h"
#include <string>

void Window::Init(const WinInitSettings& init)
{ 
    FILE* consoleOut;
    AllocConsole();
    freopen_s(&consoleOut,"CONOUT$","w",stdout);
    //freopen_s(&consoleOut, "CONOUT$", "w",stderr);
    setvbuf(consoleOut,nullptr,_IONBF,1024);

    HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow,&consoleSize);
    MoveWindow(consoleWindow,consoleSize.left,consoleSize.top,1280,720,true);


	moduleHandler = init.hInstance;

	constexpr LPCWSTR windowClassName = L"ShipyardEditorWindow";

	// First we create our Window Class
	WNDCLASS windowClass = {};
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hCursor = LoadCursor(nullptr,IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	// Then we use the class to create our window
	windowHandler = CreateWindow(
		windowClassName,                                // Classname
		init.windowTitle,                                    // Window Title
		WS_OVERLAPPEDWINDOW | WS_POPUP,    // Flags
		(GetSystemMetrics(SM_CXSCREEN) - init.windowSize.cx) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - init.windowSize.cy) / 2,
		init.windowSize.cx,
		init.windowSize.cy,
		nullptr,nullptr,nullptr,
		nullptr
	);

	//TODO error repport on fail
}

bool Window::Update()
{
	MSG msg{};
	while(PeekMessage(&msg,nullptr,0,0,PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(msg.message == WM_QUIT) return false;
	}
	return true; 
}
LRESULT CALLBACK Window::WinProc(_In_ HWND hWnd,_In_ UINT uMsg,_In_ WPARAM wParam,_In_ LPARAM lParam)
{
	LRESULT out{};
	if(uMsg == WM_DESTROY || uMsg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return 0;
	}
	out = DefWindowProc(hWnd,uMsg,wParam,lParam);
	if(callback)
	{
		MSG msg{};
		msg.hwnd = hWnd;
		msg.message = uMsg;
		msg.wParam = wParam;
		msg.lParam = lParam;

		callback(msg);
	}
	return out;
}
void Window::SetCallbackFunction(std::function<void( MSG const& msg)> aCallback)
{
	callback = aCallback;
}

void Window::Destroy()
{
	callback = nullptr;
	if(windowHandler) ::DestroyWindow(windowHandler);
}
