#pragma once
#include "EditorWindow.h"
#include <Tools/ImGui/imgui.h>
#include <Tools/Logging/Logging.h>
#include <array>
#include <functional>
#include <future>

class CustomFuncWindow : public EditorWindow
{
public:
	CustomFuncWindow() = delete;

	// Sorry, you have to input the default arguments as part of the constructor
	template <typename CallbackFunction, typename... Args>
	explicit CustomFuncWindow(const char* name, int flags, const CallbackFunction&& callback, Args... someArguments) : EditorWindow(name, flags)

	{
		for (auto& letter : uniqueID)
		{
			letter = static_cast<char>(rand() & 255);
		}

		m_Func = [=] { callback(someArguments...); };

		if (!m_Func)
		{
			LOGGER.Err("CustomFuncWindow::CustomFuncWindow: m_Func is empty");
		}
	}

	void RenderImGUi() override
	{
		if (m_Func)
		{
			m_Func();
		}
	};

private:
	std::function<void()>        m_Func;
	std::array<unsigned char, 8> uniqueID;
};
