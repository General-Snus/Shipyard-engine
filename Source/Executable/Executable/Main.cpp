
#include "Editor/Editor/Core/Editor.h"
#include "Editor/Editor/Windows/Window.h" 
#include <External/dpp/dpp.h>
#include <Windows.h> // Mean and lean is in compiler defines

#include <Strsafe.h>
#include <Tools/Utilities/System/ServiceLocator.h>
#include <dbghelp.h> 

extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = 615; // This is a hilarously bad approach, but sure microsoft ill bite
}

extern "C"
{
    __declspec(dllexport) extern const char *D3D12SDKPath = ".\\D3D12\\";
}
// #define GuardedMain

LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS *aExceptionP);
void CreateMiniDump(EXCEPTION_POINTERS *someExceptionPointers);
int Run(HINSTANCE &hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    if (IsDebuggerPresent())
    {
        Run(hInstance);
    }
    else
    {
        __try
        {
            Run(hInstance);
        }
        __except (ExceptionFilterFunction(GetExceptionInformation()))
        {
        }
    }
    return 0;
}

int Run(HINSTANCE &hInstance)
{
    auto &window = ServiceLocator::Instance().ProvideService<Window>();
    ServiceLocator::Instance().ProvideService<Editor>();

    WinInitSettings settings{};
    settings.hInstance = hInstance;
    settings.windowSize = {1920/2, 1080/2};
    settings.windowTitle = L"Shipyard";
    window.Init(settings);

    Editor &editor = ServiceLocator::Instance().GetService<Editor>();
    editor.Initialize(window.windowHandler);
    window.SetCallbackFunction([&editor](Window* win,MSG const &msg_data) { editor.DoWinProc(win,msg_data); });

    ShowWindow(WindowInstance.windowHandler, SW_SHOW);
    SetForegroundWindow(WindowInstance.windowHandler);

    while (window.Update())
    {
        editor.Run();
    }

    window.Destroy();
    return 1;
};
LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS *aExceptionP)
{
    CreateMiniDump(aExceptionP);
    return EXCEPTION_EXECUTE_HANDLER;
}

void CreateMiniDump(EXCEPTION_POINTERS *someExceptionPointers)
{
    BOOL bMiniDumpSuccessful;
    WCHAR szPath[MAX_PATH];
    WCHAR szFileName[MAX_PATH];
    WCHAR szDirectoryName[MAX_PATH];
    const WCHAR *szAppName = L"Shipyard";
    ;
    DWORD dwBufferSize = MAX_PATH;
    HANDLE hDumpFile;
    SYSTEMTIME sysTime;
    MINIDUMP_EXCEPTION_INFORMATION ExpParam;
    WCHAR bmpFileName[MAX_PATH];

    GetLocalTime(&sysTime);
    GetTempPath(dwBufferSize, szPath);

    // Creating here because windows defender is angsty about me writing anywhere else apparently
    // Apparently fine to copy beause it have greater controll then??

    StringCchPrintf(szDirectoryName, MAX_PATH, L"%s%s\\%s_%04d%02d%02d-%02d%02d", szPath, szAppName, szAppName,
                    sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute);

    CreateDirectory(szDirectoryName, NULL);
    std::filesystem::create_directories(szDirectoryName);
    StringCchPrintf(szFileName, MAX_PATH, L"%s%s", szDirectoryName, L"\\MiniDump.dmp");
    StringCchPrintf(bmpFileName, MAX_PATH, L"%s%s", szDirectoryName, L"\\Screendump.bmp");

    hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0,
                           CREATE_ALWAYS, 0, 0);
    ExpParam.ThreadId = GetCurrentThreadId();
    ExpParam.ExceptionPointers = someExceptionPointers;
    ExpParam.ClientPointers = TRUE;

    bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
                                            MiniDumpWithIndirectlyReferencedMemory, &ExpParam, NULL, NULL);
    // Straight up screen dump
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    BITMAP bAllDesktops;
    HDC hDC, hMemDC;
    LONG lWidth, lHeight;
    BYTE *bBits = NULL;
    DWORD cbBits, dwWritten = 0;
    HANDLE hFile;
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bfHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader, sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo, sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    hDC = GetDC(NULL);
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    lWidth = bAllDesktops.bmWidth;
    lHeight = bAllDesktops.bmHeight;

    DeleteObject(hTempBitmap);

    bfHeader.bfType = (WORD)('B' | ('M' << 8));
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight;

    bInfo.bmiHeader = biHeader;

    cbBits = (((24 * lWidth + 31) & ~31) / 8) * lHeight;

    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);
    hFile = CreateFileW(bmpFileName, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
        WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
        WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);
        FlushFileBuffers(hFile);
    }
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    std::filesystem::path directory = szDirectoryName;
    std::filesystem::path newDirectory = std::filesystem::current_path();
    newDirectory.operator+=("\\..\\..\\Logs\\" + directory.filename().string());
    std::filesystem::create_directories(newDirectory);
    std::error_code arg;
    if (std::filesystem::exists(newDirectory))
    {
        auto dur = std::chrono::duration<double, std::milli>(500.0);
        for (int i = 1; i <= 5; ++i)
        {
            try
            {
                std::filesystem::copy(
                    directory, newDirectory, // Error is here
                    std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, arg);
                break;
            }
            catch (std::exception e)
            {
                if (i <= 5)
                {
                    std::cout << arg.message() << " \n\n";
                    std::this_thread::sleep_for(dur);
                }
            }
        }
    }
    std::cout << arg.message() << " \n\n";

    std::wstring message = L"I CRASHED? IMPOSSIBLE!\nLog can be found at ";
    message.append(std::filesystem::absolute(newDirectory).wstring());
    message.append(L"\nPlease pray for me!!");

    MessageBeep(MB_ICONERROR); // funny
    MessageBox(NULL, message.c_str(), L"DAMMNATION!", MB_ICONERROR | MB_OK);

    dpp::cluster bot("");
    dpp::webhook wh("https://discord.com/api/webhooks/1179732203417120838/"
                    "3K8hA2cj4heYzeuqPx8uBQSHKxSDw7iZynL4XV8C9YH7jlbfr6kiKSaXp-gvBr-PtZpI");

    // create a message
    dpp::message msg1("The program crashed! Here are the logs!");
    auto filePath1 = newDirectory.string() + "\\MiniDump.dmp";
    msg1.add_file("MiniDump.dmp", dpp::utility::read_file(filePath1));
    bot.execute_webhook(wh, msg1, true);

    return;
}
