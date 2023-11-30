
#include "resource.h"
#include "Core/Editor.h" 
#include "Windows/Window.h" 
#include <Windows.h> // Mean and lean is in compiler defines
#include <Tools/ThirdParty/dpp/dpp.h>

#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <Strsafe.h>
#include <stdio.h>
#include <wchar.h>

LRESULT CALLBACK WinProc(_In_ HWND hWnd,_In_ UINT uMsg,_In_ WPARAM wParam,_In_ LPARAM lParam);
LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP);
void CreateMiniDump(EXCEPTION_POINTERS* someExceptionPointers);
int Run(HINSTANCE& hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);


	__try
	{
		Run(hInstance);
	}
	__except(ExceptionFilterFunction(GetExceptionInformation()))
	{
	}
	return true;
}

int Run(HINSTANCE& hInstance)
{
	WinInitSettings settings{};
	settings.hInstance = hInstance;
	settings.windowSize = {1920, 1080};
	settings.windowTitle = L"Shipyard";
	Window::Init(settings);

	Editor editor;
	editor.Initialize(Window::windowHandler);
	Window::SetCallbackFunction([&editor](MSG const& msg_data) { editor.DoWinProc(msg_data); });
	while(Window::Update())
	{
		editor.Run();
	}
	Window::Destroy();
	return 1;
};

LONG WINAPI ExceptionFilterFunction(_EXCEPTION_POINTERS* aExceptionP)
{
	CreateMiniDump(aExceptionP);
	return EXCEPTION_EXECUTE_HANDLER;
}

void CreateMiniDump(EXCEPTION_POINTERS* someExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	WCHAR szPath[MAX_PATH];
	WCHAR szFileName[MAX_PATH];
	WCHAR szDirectoryName[MAX_PATH];
	const WCHAR* szAppName = L"Shipyard";
	const WCHAR* szTitle = L"ShipyardLog";;
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME sysTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
	WCHAR bmpFileName[MAX_PATH];

	GetLocalTime(&sysTime);
	GetTempPath(dwBufferSize,szPath);

	// Creating here because windows defender is angsty about me writing anywhere else apparently
	// Apparently fine to copy beause it have greater controll then??


	StringCchPrintf(szDirectoryName,MAX_PATH,L"%s%s\\%s_%04d%02d%02d-%02d%02d",
		szPath,szAppName,szTitle,
		sysTime.wYear,sysTime.wMonth,sysTime.wDay,
		sysTime.wHour,sysTime.wMinute);

	CreateDirectory(szDirectoryName,NULL);
	StringCchPrintf(szFileName,MAX_PATH,L"%s%s",szDirectoryName,L"\\MiniDump.dmp");
	StringCchPrintf(bmpFileName,MAX_PATH,L"%s%s",szDirectoryName,L"\\Screendump.bmp");

	hDumpFile = CreateFile(szFileName,GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,0,CREATE_ALWAYS,0,0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = someExceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),
		hDumpFile,MiniDumpWithDataSegs,&ExpParam,NULL,NULL);


	//Straight up screen dump
	BITMAPFILEHEADER bfHeader;
	BITMAPINFOHEADER biHeader;
	BITMAPINFO bInfo;
	HGDIOBJ hTempBitmap;
	HBITMAP hBitmap;
	BITMAP bAllDesktops;
	HDC hDC,hMemDC;
	LONG lWidth,lHeight;
	BYTE* bBits = NULL;
	DWORD cbBits,dwWritten = 0;
	HANDLE hFile;
	INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

	ZeroMemory(&bfHeader,sizeof(BITMAPFILEHEADER));
	ZeroMemory(&biHeader,sizeof(BITMAPINFOHEADER));
	ZeroMemory(&bInfo,sizeof(BITMAPINFO));
	ZeroMemory(&bAllDesktops,sizeof(BITMAP));

	hDC = GetDC(NULL);
	hTempBitmap = GetCurrentObject(hDC,OBJ_BITMAP);
	GetObjectW(hTempBitmap,sizeof(BITMAP),&bAllDesktops);

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
	hBitmap = CreateDIBSection(hDC,&bInfo,DIB_RGB_COLORS,(VOID**)&bBits,NULL,0);
	SelectObject(hMemDC,hBitmap);
	BitBlt(hMemDC,0,0,lWidth,lHeight,hDC,x,y,SRCCOPY);
	hFile = CreateFileW(bmpFileName,GENERIC_WRITE | GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		DeleteDC(hMemDC);
		ReleaseDC(NULL,hDC);
		DeleteObject(hBitmap);
		return;
	}
	WriteFile(hFile,&bfHeader,sizeof(BITMAPFILEHEADER),&dwWritten,NULL);
	WriteFile(hFile,&biHeader,sizeof(BITMAPINFOHEADER),&dwWritten,NULL);
	WriteFile(hFile,bBits,cbBits,&dwWritten,NULL);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);
	DeleteDC(hMemDC);
	ReleaseDC(NULL,hDC);
	DeleteObject(hBitmap);

	std::filesystem::path directory = szDirectoryName;
	std::filesystem::path newDirectory = std::filesystem::current_path();
	newDirectory.operator+=("\\..\\..\\Logs\\" + directory.filename().string());
	std::filesystem::create_directories(newDirectory);
	if(std::filesystem::exists(newDirectory))
	{
		std::filesystem::copy(directory,newDirectory,
			std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
	}


	std::wstring message = L"I CRASHED? IMPOSSIBLE!\nLog can be found at ";
	message.append(std::filesystem::absolute(newDirectory).wstring());
	message.append(L"\nPlease pray for me!!");

	MessageBeep(MB_ICONERROR); //funny
	MessageBox(NULL,message.c_str(),L"DAMMNATION!",
		MB_ICONERROR | MB_OK);

	dpp::cluster bot("");
	dpp::webhook wh("https://discord.com/api/webhooks/1179732203417120838/3K8hA2cj4heYzeuqPx8uBQSHKxSDw7iZynL4XV8C9YH7jlbfr6kiKSaXp-gvBr-PtZpI");

	// create a message
	dpp::message msg1("The program crashed! Here are the logs!"); 
	auto filePath1 = newDirectory.string() + "\\MiniDump.dmp";  
	msg1.add_file("MiniDump.dmp",dpp::utility::read_file(filePath1)); 
	bot.execute_webhook(wh,msg1,true); 
	 
	return;

}