#define WIN32_LEAN_AND_MEAN 
#include "resource.h"
#include "Core/Editor.h" 
#include "Windows/Window.h"

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

    WinInitSettings settings{};
    settings.hInstance = hInstance;
    settings.windowSize = { 1920, 1080 };
    settings.windowTitle = L"Shipyard"; 
    Window::Init(settings);

    //Here is the difference between the editor and gamelauncher
    //Editor editor;

    Editor editor;
    editor.Initialize(Window::windowHandler);
    Window::SetCallbackFunction([&editor](MSG const& msg_data) { editor.DoWinProc(msg_data); });
    while(Window::Update())
    {
        editor.Run();
    }
    return true;
}

