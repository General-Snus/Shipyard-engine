#pragma once
#define NOMINMAX
#include <bitset>
#include <Windows.h>
#include <WinUser.h>
#include <Tools/Utilities/System/ServiceLocator.h>
#include "EnumKeys.h"
#include "../LinearAlgebra/Vector2.hpp"
#include "Editor/Editor/Windows/Window.h"
#include "Tools/Logging/Logging.h"

#define Input ServiceLocator::Instance().GetService<InputManager>()

class InputManager : public Singleton
{
public:
	InputManager();

	bool                  IsKeyHeld(int aKeyCode) const;
	bool                  IsKeyHeld(Keys aKeyCode) const;
	bool                  IsKeyPressed(int aKeyCode) const;
	bool                  IsKeyPressed(Keys aKeyCode) const;
	bool                  IsKeyReleased(int aKeyCode) const;
	bool                  IsKeyReleased(Keys aKeyCode) const;
	Vector2<float>        GetMousePositionNDC() const;
	static Vector2<float> GetMousePositionNDC(const Vector2<float>& mp);
	Vector2f              GetMousePosition() const;
	Vector2f              GetMousePositionDelta() const;
	bool                  UpdateMouseInput();
	// Positive is forward away from user
	float        GetMouseWheelDelta() const;
	unsigned int GetLastPressedKey() const;
	bool         UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
	void         Update();
	float        g_MouseWheelDeadZone = 10;

private:
	HWND hwnd;
	std::bitset<256> currentFrameUpdate;
	std::bitset<256> liveKeyUpdate;
	std::bitset<256> lastFrameUpdate;
	unsigned int     lastPressedKey{};
	float            myMouseWheelDelta{};
	Vector2<float>   myMousePosition;
	Vector2<float>   myLastMousePosition;
};

inline InputManager::InputManager()
{
	hwnd = WindowInstance.windowHandler;
}

inline bool InputManager::IsKeyHeld(const int aKeyCode) const
{
	return (currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}

inline bool InputManager::IsKeyHeld(const Keys aKeyCode) const
{
	return (currentFrameUpdate[static_cast<int>(aKeyCode)] && lastFrameUpdate[static_cast<int>(aKeyCode)]);
}

inline bool InputManager::IsKeyPressed(const int aKeyCode) const
{
	return (currentFrameUpdate[aKeyCode] && !lastFrameUpdate[aKeyCode]);
}

inline bool InputManager::IsKeyPressed(const Keys aKeyCode) const
{
	return (currentFrameUpdate[static_cast<int>(aKeyCode)] && !lastFrameUpdate[static_cast<int>(aKeyCode)]);
}

inline bool InputManager::IsKeyReleased(const int aKeyCode) const
{
	return (!currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}

inline bool InputManager::IsKeyReleased(const Keys aKeyCode) const
{
	return (!currentFrameUpdate[static_cast<int>(aKeyCode)] && lastFrameUpdate[static_cast<int>(aKeyCode)]);
}

inline Vector2<float> InputManager::GetMousePositionNDC() const
{
	const Vector2ui res = { WindowInstance.Width(), WindowInstance.Height() };
	return Vector2f(std::clamp(2.f * (myMousePosition.x / static_cast<float>(res.x)) - 1.0f, -1.0f, 1.0f),
		std::clamp(1.0f - 2.f * (myMousePosition.y / static_cast<float>(res.y)), -1.0f, 1.0f));
}

inline Vector2<float> InputManager::GetMousePositionNDC(const Vector2<float>& mp)
{
	const Vector2ui res = { WindowInstance.Width(), WindowInstance.Height() };
	return Vector2f(std::clamp(2.f * (mp.x / static_cast<float>(res.x)) - 1.0f, -1.0f, 1.0f),
		std::clamp(1.0f - 2.f * (mp.y / static_cast<float>(res.y)), -1.0f, 1.0f));
}

inline Vector2<float> InputManager::GetMousePosition() const
{
	return myMousePosition;
}

inline Vector2<float> InputManager::GetMousePositionDelta() const
{
	return (GetMousePositionNDC(myMousePosition) - GetMousePositionNDC(myLastMousePosition));
}

inline float InputManager::GetMouseWheelDelta() const
{
	return myMouseWheelDelta;
}

inline bool InputManager::UpdateMouseInput()
{
	myLastMousePosition = myMousePosition;

	return true;
}

inline unsigned int InputManager::GetLastPressedKey() const
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
		lastPressedKey = static_cast<unsigned int>(wParam);
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
	{
		POINT pt = { 0, 0 };
		if (GetCursorPos(&pt)) {
			//if (ScreenToClient(hwnd, &pt))
			{
				myMousePosition.x = static_cast<float>(pt.x);
				myMousePosition.y = static_cast<float>(pt.y); 
			}
		}

		return true;
	}

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
