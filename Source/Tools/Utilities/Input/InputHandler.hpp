#pragma once
#define NOMINMAX

#include <bitset>
#include <Windows.h>
#include <WinUser.h>
#include "../LinearAlgebra/Vector2.hpp"
#include "EnumKeys.h"

class InputHandler
{
public:
	InputHandler() = default;

	static InputHandler& GetInstance()
	{
		static InputHandler arg;
		return  arg;
	};

	bool IsKeyHeld(const int aKeyCode) const;
	bool IsKeyPressed(const int aKeyCode) const;
	bool IsKeyReleased(const int aKeyCode) const;
	Vector2<int> GetMousePosition() const;
	Vector2<int> GetMousePositionDelta() const;
	bool UpdateMouseInput(UINT message);
	unsigned int GetLastPressedKey() const;

	bool UpdateEvents(UINT message,WPARAM wParam,LPARAM lParam);
	void Update();

private:
	// The last current snapshot when we last ran Update.
	std::bitset<256> currentFrameUpdate;

	std::bitset<256> liveKeyUpdate;

	// The previous snapshot.
	std::bitset<256> lastFrameUpdate;


	unsigned int lastPressedKey;
	Vector2<int> myMousePosition;
	Vector2<int> myLastMousePosition;

};

inline bool InputHandler::IsKeyHeld(const int aKeyCode) const
{
	return (currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}

inline bool InputHandler::IsKeyPressed(const int aKeyCode) const
{
	return (currentFrameUpdate[aKeyCode] && !lastFrameUpdate[aKeyCode]);
}

inline bool InputHandler::IsKeyReleased(const int aKeyCode) const
{
	return (!currentFrameUpdate[aKeyCode] && lastFrameUpdate[aKeyCode]);
}

inline Vector2<int> InputHandler::GetMousePosition() const
{
	return myMousePosition;
}

inline Vector2<int> InputHandler::GetMousePositionDelta() const
{
	return  (myMousePosition - myLastMousePosition);
}

inline bool InputHandler::UpdateMouseInput(UINT message)
{
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

	case WM_MOUSEMOVE:
		POINT pt = { 0, 0 };
		GetCursorPos(&pt);
		myLastMousePosition = myMousePosition;
		myMousePosition = { pt.x,pt.y };
		return true;
	}

	return false;
}

inline unsigned int InputHandler::GetLastPressedKey() const
{
	return lastPressedKey;
}

inline bool InputHandler::UpdateEvents(UINT message,WPARAM wParam,LPARAM lParam)
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

inline void InputHandler::Update()
{
	lastFrameUpdate = currentFrameUpdate;
	currentFrameUpdate = liveKeyUpdate;
}