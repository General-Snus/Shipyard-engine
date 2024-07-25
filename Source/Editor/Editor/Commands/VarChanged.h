#pragma once
#include "CommandBuffer.h"
#include <Engine/AssetManager/ComponentSystem/Gameobject.h>
#include <Tools/Logging/Logging.h>
#include <Tools/Reflection/refl.hpp>
#include <memory>

class Component;

template <typename ComponentType, typename var> class VarChanged : public BaseCommand
{
  public:
    VarChanged(ComponentType *object, const var *value, const var oldVal, const var newValue,
               const std::string &varName = "");
    void Undo() override;
    void Do() override;

    bool Merge(std::shared_ptr<BaseCommand> &ptr) override;
    std::string GetDescription() const override;

  private:
    std::string m_VariableName = "UnknownVariable";
    const GameObject m_object;
    size_t addressOffset;
    var m_OldValue;
    var m_NewValue;
};

template <typename ComponentType, typename var>
inline bool VarChanged<ComponentType, var>::Merge(std::shared_ptr<BaseCommand> &ptr)
{
    if (!BaseCommand::Merge(ptr))
    {
        return false;
    }

    if (auto otherCommand = std::dynamic_pointer_cast<VarChanged<ComponentType, var>, BaseCommand>(ptr))
    {
        const bool sameObject = m_object == otherCommand->m_object;
        const bool sameAddress = addressOffset == otherCommand->addressOffset;

        if (sameObject && sameAddress)
        {
            otherCommand->m_NewValue = m_NewValue;
            return true;
        }
    }

    return false;
}

template <typename ComponentType, typename var>
inline VarChanged<ComponentType, var>::VarChanged(ComponentType *object, const var *value, const var oldVal,
                                                  const var newValue, const std::string &varName)
    : m_object(object->GetGameObject()), m_VariableName(varName)
{
    if (!object)
    {
        Logger::Err("Could not find component");
        return;
    }

    if (m_VariableName == "")
    {
        if constexpr (refl::is_reflectable<var>(oldVal))
        {
            m_VariableName = refl::reflect<var>(oldVal).name.str();
        }
        else
        {
            m_VariableName = "UnknownVariable";
        }
    }

    addressOffset = reinterpret_cast<const char *>(value) - reinterpret_cast<char *>(&*object);
    m_OldValue = oldVal;
    m_NewValue = newValue;
}

template <typename ComponentType, typename var> inline void VarChanged<ComponentType, var>::Undo()
{
    Component *component = m_object.TryGetComponent<ComponentType>();
    if (!component)
    {
        Logger::Warn("Could not find component to undo");
        return;
    }

    const auto &address = reinterpret_cast<char *>(&*component);
    *reinterpret_cast<var *>(address + addressOffset) = m_OldValue;
}

template <typename ComponentType, typename var> inline void VarChanged<ComponentType, var>::Do()
{
    Component *component = m_object.TryGetComponent<ComponentType>();
    if (!component)
    {
        Logger::Warn("Could not find component to redo");
        return;
    }
    const auto &address = reinterpret_cast<char *>(&*component);
    *reinterpret_cast<var *>(address + addressOffset) = m_NewValue;
}

template <typename ComponentType, typename var>
inline std::string VarChanged<ComponentType, var>::GetDescription() const
{

    // if constexpr (refl::is_reflectable()<var>())
    {
        const std::string outMessage =
            std::format("{}:{} changed from {} to {}", refl::reflect<ComponentType>().name.str(), m_VariableName,
                        refl::runtime::debug_str(m_OldValue, true), refl::runtime::debug_str(m_NewValue, true));

        return outMessage;
    }
    // return "";
}
