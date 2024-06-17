#pragma once
#define NOMINMAX 
#include <bitset>
#include <Windows.h>
#include <WinUser.h>
#include "../LinearAlgebra/Vector2.hpp"
#include "Editor/Editor/Windows/Window.h"
#include "EnumKeys.h"

class Input
{
public:
	Input() = default;

	static bool IsKeyHeld(const int aKeyCode);
	static bool IsKeyHeld(Keys aKeyCode);
	static bool IsKeyPressed(const int aKeyCode);
	static bool IsKeyPressed(Keys aKeyCode);
	static bool IsKeyReleased(const int aKeyCode);
	static bool IsKeyReleased(Keys aKeyCode);
	static Vector2<float> GetMousePosition();
	static Vector2<float> GetMousePositionDelta();
	static bool UpdateMouseInput(UINT message,WPARAM wParam);
	//Positive is forward away from user
	static float GetMouseWheelDelta();
	static unsigned int GetLastPressedKey();
	static bool UpdateEvents(UINT message,WPARAM wParam,LPARAM lParam);
	static void Update();

private:
	inline static std::bitset<256> currentFrameUpdate;
	inline static std::bitset<256> liveKeyUpdate;
	inline static std::bitset<256> lastFrameUpdate;
	inline static unsigned int lastPressedKey;
	inline static float myMouseWheelDelta;
	inline static Vector2<float> myMousePosition;
	inline static Vector2<float> myLastMousePosition;

};

inline bool Input::IsKeyHeld(const int aKeyCode)
{
	return (currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}
inline bool Input::IsKeyHeld(const Keys aKeyCode)
{
	return (currentFrameUpdate[(int)aKeyCode] && lastFrameUpdate[(int)aKeyCode]);
}


inline bool Input::IsKeyPressed(const int aKeyCode)
{
	return (currentFrameUpdate[aKeyCode] && !lastFrameUpdate[aKeyCode]);
}

inline bool Input::IsKeyPressed(const Keys aKeyCode)
{
	return (currentFrameUpdate[(int)aKeyCode] && !lastFrameUpdate[(int)aKeyCode]);
}

inline bool Input::IsKeyReleased(const int aKeyCode)
{
	return (!currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}
inline bool Input::IsKeyReleased(const Keys aKeyCode)
{
	return (!currentFrameUpdate[(int)aKeyCode] && lastFrameUpdate[(int)aKeyCode]);
}


inline Vector2<float> Input::GetMousePosition()
{
	return myMousePosition;
}

inline Vector2<float> Input::GetMousePositionDelta()
{
	return  (myMousePosition - myLastMousePosition);
}
inline float Input::GetMouseWheelDelta()
{
	return myMouseWheelDelta;
}

inline bool Input::UpdateMouseInput(UINT message,WPARAM wParam)
{
	myLastMousePosition = myMousePosition;
	POINT pt = { 0, 0 };
	GetCursorPos(&pt);
	const Vector2ui res = { Window::Width(),Window::Height() };
	myMousePosition.x = 1.f - 2.f * (static_cast<float>(pt.x) / static_cast<float>(res.x));
	myMousePosition.y = 1.0f - 2.f * (static_cast<float>(pt.y) / static_cast<float>(res.y));
	myMousePosition.x *= -1.f;

	switch (message)
	{
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

inline unsigned int Input::GetLastPressedKey()
{
	return lastPressedKey;
}

inline bool Input::UpdateEvents(UINT message,WPARAM wParam,LPARAM lParam)
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
	default:
		return false;
	}
}

inline void Input::Update()
{
	lastFrameUpdate = currentFrameUpdate;
	currentFrameUpdate = liveKeyUpdate;
	myMouseWheelDelta = 0.0f;
}