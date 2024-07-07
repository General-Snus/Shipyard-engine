#pragma once
#include "EditorWindow.h"
#include <functional>
#include <future>
#include <Tools/Logging/Logging.h>
#include <imgui.h>

class CustomFuncWindow : public EditorWindow
{
public:
	CustomFuncWindow() = delete;

	//Sorry, you have to input the default arguments as part of the constructor
	template <typename CallbackFunction,typename ...Args>
	explicit CustomFuncWindow(const CallbackFunction&& callback,Args... someArguments)
	{
		m_Func = [=]()
			{
				callback(someArguments...);
			};

		if (!m_Func)
		{
			Logger::Err("CustomFuncWindow::CustomFuncWindow: m_Func is empty");
		}
	};

	void RenderImGUi()
	{
		ImGui::Begin("CustomFuncWindow",&m_KeepWindow);
		if (m_Func)
		{
			m_Func();
		}
		ImGui::End();
	};

private:
	std::function<void()> m_Func;

};