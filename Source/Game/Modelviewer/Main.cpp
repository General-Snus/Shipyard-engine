#define WIN32_LEAN_AND_MEAN
#include "Windows.h" 
#include "resource.h"
#include "Core/Modelviewer.h" 

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    constexpr SIZE windowSize = { 1920, 1080 };
    constexpr LPCWSTR windowTitle = L"Shipyard";

    ModelViewer& MV = ModelViewer::Get();
    
    FILE* consoleOut;
    AllocConsole();
    freopen_s(&consoleOut, "CONOUT$", "w", stdout);
    //freopen_s(&consoleOut, "CONOUT$", "w",stderr);
    setvbuf(consoleOut, nullptr, _IONBF, 1024);

	HWND consoleWindow = GetConsoleWindow();
    RECT consoleSize;
    GetWindowRect(consoleWindow, &consoleSize);
    MoveWindow(consoleWindow, consoleSize.left, consoleSize.top, 1280, 720, true);

    MV.Initialize(hInstance, windowSize, WinProc, windowTitle);
    return MV.Run();
}

LRESULT CALLBACK WinProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    if (uMsg == WM_DESTROY || uMsg == WM_CLOSE)
    {
        PostQuitMessage(0);
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}