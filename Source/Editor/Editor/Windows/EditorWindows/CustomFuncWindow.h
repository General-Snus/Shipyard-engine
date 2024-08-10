#pragma once
#include "EditorWindow.h"
#include <Tools/ImGui/ImGui/imgui.h>
#include <Tools/Logging/Logging.h>
#include <functional>
#include <future>

class CustomFuncWindow : public EditorWindow
{
  public:
    CustomFuncWindow() = delete;

    // Sorry, you have to input the default arguments as part of the constructor
    template <typename CallbackFunction, typename... Args>
    explicit CustomFuncWindow(const CallbackFunction &&callback, Args... someArguments)

    {
        m_Func = [=]() { callback(someArguments...); };

        if (!m_Func)
        {
            Logger::Err("CustomFuncWindow::CustomFuncWindow: m_Func is empty");
        }
    };

    void SetWindowName(const std::string& aName);

    void RenderImGUi()
    {
        ImGui::PushID((void *)this);
        ImGui::Begin(windowsName.c_str(), &m_KeepWindow);
        if (m_Func)
        {
            m_Func();
        }
        ImGui::End();

        ImGui::PopID();
    };

  private:
    std::function<void()> m_Func;
    std::string windowsName;
};
