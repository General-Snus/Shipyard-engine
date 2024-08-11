#pragma once
#include <functional>

// How to use
// Use a member variable in a class to act as a eventsystem
// Example: Editor stores a OnObjectSelected event, HierarchyWindow Invoke, Inspector Listens
// Use as argument, you can use the event class to transfer authority of invokation
class Event
{
    using callback = std::function<void()>;

  public:
    Event() = default;

    template <typename F, typename... Args> void AddListener(F &&f, Args &&...args)
    {
        m_CallbackFunctions.push_back(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    }

    void RemoveListener(callback &func);
    void Invoke();

  private:
    std::vector<callback> m_CallbackFunctions;
};
