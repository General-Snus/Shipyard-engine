#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <Tools/Utilities/System/SingletonTemplate.h>
#include <functional>
#include <string>
#include <windows.h>
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
    static LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetCallbackFunction(const std::function<void(MSG const &msg)> &aCallback);
    void Destroy();
    void MoveConsoleToOtherMonitor();

    unsigned int Width() const;
	unsigned int Height() const;
	Vector2ui Resolution() const;

    HINSTANCE moduleHandler;
    HWND windowHandler;
    // Cred goes to adira guy on reddit for wonderfull code
    std::function<void(const MSG &msg)> callback = nullptr;
};
