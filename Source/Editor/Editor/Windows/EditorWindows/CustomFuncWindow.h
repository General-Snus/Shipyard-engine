#pragma once
#include "EditorWindow.h"
#include <Tools/ImGui/imgui.h>
#include <Tools/Logging/Logging.h>
#include <functional>
#include <future>
#include <array>

class CustomFuncWindow : public EditorWindow
{
  public:
    CustomFuncWindow() = delete;

    // Sorry, you have to input the default arguments as part of the constructor
    template <typename CallbackFunction, typename... Args>
    explicit CustomFuncWindow(const CallbackFunction &&callback, Args... someArguments)

    {
		for (auto &letter : uniqueID)
		{
			letter = (char)(rand() & 255);  
		}

        m_Func = [=]() { callback(someArguments...); };

        if (!m_Func)
        {
            Logger.Err("CustomFuncWindow::CustomFuncWindow: m_Func is empty");
        }
    };

    void SetWindowName(const std::string &aName);

    void RenderImGUi()
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
    std::function<void()> m_Func;
    std::string windowsName;
    std::array<unsigned char,8> uniqueID;
};
