#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <functional>
#include <string>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif 
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>


#define WindowInstance ServiceLocator::Instance().GetService<Window>()

class DropManager;
struct WinInitSettings
{
    std::wstring windowTitle;
    Vector2ui windowSize;
    HINSTANCE hInstance;
};

class Window : public Singleton
{
  public:
    void Init(const WinInitSettings &init);
    bool Update();
    LRESULT static CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetCallbackFunction(const std::function<void(Window* window,MSG const &msg)> &aCallback);
    void Destroy();
    void MoveConsoleToOtherMonitor();

    unsigned int Width() const;
	unsigned int Height() const;
	Vector2ui Resolution() const;


    unsigned int MonitorWidth() const;
    unsigned int MonitorHeight() const;
    Vector2ui MonitorResolution() const;

	bool SetWindowsTitle(const std::string& titleName) const;
	std::string GetWindowsTitle() const;

    HINSTANCE moduleHandler{};
	HWND windowHandler{};
    // Cred goes to adira guy on reddit for wonderfull code
    std::function<void(Window* window,const MSG &msg)> callback = nullptr;
};
