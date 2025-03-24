#include "../SplashWindow.h"
#include "Executable/Executable/Resource.h"
#include <thread>

LRESULT SplashWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void SplashWindow::CreateAndShowWindow()
{
    bmp = LoadBitmap(myHInstance, MAKEINTRESOURCE(IDB_SPLASHLOGO));
    const DWORD LastError = GetLastError();
    LastError;
    WNDCLASS splashClass = {};
    splashClass.style = 0;
    splashClass.lpfnWndProc = SplashWinProc;
    splashClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    splashClass.lpszClassName = L"SplashScreen";
    RegisterClass(&splashClass);

    constexpr SIZE windowSize = {800, 500};

    myHandle =
        CreateWindow(L"SplashScreen", NULL, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, GetModuleHandle(NULL), nullptr);

    myLayeredHandle = CreateWindowEx(WS_EX_LAYERED, L"SplashScreen", NULL, WS_POPUP | WS_VISIBLE, 0, 0, 0, 0, myHandle,
                                     NULL, GetModuleHandle(NULL), nullptr);

    BITMAP b;
    GetObject(bmp, sizeof(BITMAP), &b);
    SIZE splashScreenSize = {b.bmWidth, b.bmHeight};

    POINT ptZero = {0};
    const HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorinfo = {0};
    monitorinfo.cbSize = sizeof(monitorinfo);
    GetMonitorInfo(hmonPrimary, &monitorinfo);

    const RECT &rcWork = monitorinfo.rcWork;
    POINT ptOrigin;
    ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - splashScreenSize.cx) / 2;
    ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - splashScreenSize.cy) / 2;

    const HDC hdcScreen = GetDC(NULL);
    const HDC hdcMem = CreateCompatibleDC(hdcScreen);
    const HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, bmp);

    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    UpdateLayeredWindow(myLayeredHandle, hdcScreen, &ptOrigin, &splashScreenSize, hdcMem, &ptZero, RGB(0, 0, 0), &blend,
                        ULW_OPAQUE);
    SelectObject(hdcMem, hbmpOld);
    DeleteDC(hdcMem);
    ReleaseDC(myHandle, hdcScreen);

    ShowWindow(myHandle, SW_SHOW);

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (bIsRunning)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_CLOSE)
            {
                bIsRunning = false;
            }
        }
    }
}

void SplashWindow::Init(HINSTANCE hInstance)
{
    myHInstance = hInstance;
    myThread = std::thread(&SplashWindow::CreateAndShowWindow, this);
}

void SplashWindow::Close()
{
    bIsRunning = false;
    myThread.join();
}

SplashWindow::~SplashWindow()
{
    UnregisterClass(L"SplashScreen", myHInstance);
}
