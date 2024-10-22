#pragma once
#define NOMINMAX
#include "../LinearAlgebra/Vector2.hpp"
#include "Editor/Editor/Windows/Window.h"
#include "EnumKeys.h"
#include "Tools/Logging/Logging.h"
#include <Tools/Utilities/System/ServiceLocator.h>
#include <WinUser.h>
#include <Windows.h>
#include <bitset>

#define Input ServiceLocator::Instance().GetService<InputManager>()

class InputManager : public Singleton
{
  public:
    InputManager() = default;

    bool IsKeyHeld(const int aKeyCode);
    bool IsKeyHeld(Keys aKeyCode);
    bool IsKeyPressed(const int aKeyCode);
    bool IsKeyPressed(Keys aKeyCode);
    bool IsKeyReleased(const int aKeyCode);
    bool IsKeyReleased(Keys aKeyCode);
    Vector2f GetMousePosition();
    Vector2f GetMousePositionDelta();
    bool UpdateMouseInput();
    // Positive is forward away from user
    float GetMouseWheelDelta();
    unsigned int GetLastPressedKey();
    bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
    void Update();
    float g_MouseWheelDeadZone = 10;

  private:
    std::bitset<256> currentFrameUpdate;
    std::bitset<256> liveKeyUpdate;
    std::bitset<256> lastFrameUpdate;
    unsigned int lastPressedKey;
    float myMouseWheelDelta;
    Vector2<float> myMousePosition;
    Vector2<float> myLastMousePosition;
};

inline bool InputManager::IsKeyHeld(const int aKeyCode)
{
    return (currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}
inline bool InputManager::IsKeyHeld(const Keys aKeyCode)
{
    return (currentFrameUpdate[(int)aKeyCode] && lastFrameUpdate[(int)aKeyCode]);
}

inline bool InputManager::IsKeyPressed(const int aKeyCode)
{
    return (currentFrameUpdate[aKeyCode] && !lastFrameUpdate[aKeyCode]);
}

inline bool InputManager::IsKeyPressed(const Keys aKeyCode)
{
    return (currentFrameUpdate[(int)aKeyCode] && !lastFrameUpdate[(int)aKeyCode]);
}

inline bool InputManager::IsKeyReleased(const int aKeyCode)
{
    return (!currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}
inline bool InputManager::IsKeyReleased(const Keys aKeyCode)
{
    return (!currentFrameUpdate[(int)aKeyCode] && lastFrameUpdate[(int)aKeyCode]);
}

inline Vector2<float> InputManager::GetMousePosition()
{
    return myMousePosition;
}

inline Vector2<float> InputManager::GetMousePositionDelta()
{
    return (myMousePosition - myLastMousePosition);
}
inline float InputManager::GetMouseWheelDelta()
{
    return myMouseWheelDelta;
}

inline bool InputManager::UpdateMouseInput()
{
    myLastMousePosition = myMousePosition;
    POINT pt = {0, 0};
    GetCursorPos(&pt);
    const Vector2ui res = {WindowInstance.Width(), WindowInstance.Height()};
    myMousePosition.x = 1.f - 2.f * (static_cast<float>(pt.x) / static_cast<float>(res.x));
    myMousePosition.y = 1.0f - 2.f * (static_cast<float>(pt.y) / static_cast<float>(res.y));
    myMousePosition.x *= -1.f;

    return true;
}

inline unsigned int InputManager::GetLastPressedKey()
{
    return lastPressedKey;
}

inline bool InputManager::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
{
    lParam;
    switch (message)
    {
    case WM_KEYDOWN:
        liveKeyUpdate[wParam] = true;
        lastPressedKey = (unsigned int)wParam;
        return true;
    case WM_KEYUP:
        liveKeyUpdate[wParam] = false;
        return true;

    case WM_LBUTTONDOWN:
        lastPressedKey = 0x01;
        liveKeyUpdate[0x01] = true;
        return true;
    case WM_LBUTTONUP:
        liveKeyUpdate[0x01] = false;
        return true;
    case WM_RBUTTONDOWN:
        lastPressedKey = 0x02;
        liveKeyUpdate[0x02] = true;
        return true;
    case WM_RBUTTONUP:
        liveKeyUpdate[0x02] = false;
        return true;

    case WM_MBUTTONDOWN:
        lastPressedKey = 0x04;
        liveKeyUpdate[0x04] = true;
        return true;

    case WM_MBUTTONUP:
        liveKeyUpdate[0x04] = false;
        return true;

    case WM_MOUSEWHEEL:
        myMouseWheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam));
        return true;

    case WM_MOUSEMOVE:
        return true;

    default:
        return false;
    }
}

inline void InputManager::Update()
{
    lastFrameUpdate = currentFrameUpdate;
    currentFrameUpdate = liveKeyUpdate;
    myMouseWheelDelta = 0.0f;
    UpdateMouseInput();
}
