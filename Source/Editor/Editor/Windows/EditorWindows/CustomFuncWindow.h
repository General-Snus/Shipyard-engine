#pragma once
#include <array>
#include <functional>
#include <future>
#include <Tools/ImGui/imgui.h>
#include <Tools/Logging/Logging.h>
#include "EditorWindow.h"

class CustomFuncWindow : public EditorWindow
{
public:
	CustomFuncWindow() = delete;

	// Sorry, you have to input the default arguments as part of the constructor
	template <typename CallbackFunction, typename... Args>
	explicit CustomFuncWindow(const CallbackFunction&& callback, Args... someArguments)

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

	void SetWindowName(const std::string& aName);

	void RenderImGUi() override
	{
		if (windowsName.empty())
		{
			windowsName = "Name your window you stupid mutt";
		}

		ImGui::Begin(std::format("{}###{}", windowsName, uniqueID).c_str(), &m_KeepWindow);
		if (m_Func)
		{
			m_Func();
		}
		ImGui::End();
	};

private:
	std::function<void()>        m_Func;
	std::string                  windowsName;
	std::array<unsigned char, 8> uniqueID;
};
