#pragma once
#include <typeinfo>
#include <unordered_map>

class SystemBase
{
  public:
    SystemBase();
    virtual ~SystemBase() = default;
    virtual void Update(float delta) = 0;
};

class SystemCollection
{
  public:
    static void AddSystem(SystemBase *system)
    {
        m_Systems.emplace(&typeid(system), system);
    }
    template <typename T> static void AddSystem(T *system)
    {
        m_Systems.emplace(&typeid(T), dynamic_cast<SystemBase>(system));
    }
    template <typename T> static void AddSystem()
    {
        m_Systems.emplace(&typeid(T), &T());
    }

    template <typename T> static T *GetSystem()
    {
        if (m_Systems.contains(&typeid(T)))
        {
            return dynamic_cast<T *>(m_Systems[&typeid(T)]);
        }
        else
        {
            auto *system = new T();
            m_Systems.emplace(&typeid(T), system);
            return system;
        }
    }

    static void UpdateSystems(float delta)
    {
        for (const auto &system : m_Systems)
        {
            system.second->Update(delta);
        }
    }

  private:
    static inline std::unordered_map<const std::type_info *, SystemBase *> m_Systems;
};
