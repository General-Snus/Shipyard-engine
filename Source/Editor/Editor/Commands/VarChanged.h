#pragma once
#include <memory>
#include <Engine/AssetManager/ComponentSystem/Gameobject.h>
#include <Tools/Logging/Logging.h>
#include <Tools/Reflection/refl.hpp>
#include "CommandBuffer.h"

#pragma region varPointerChange
template <typename var = float>
class PointerVarChanged : public BaseCommand
{
public:
	PointerVarChanged(var* value, var oldVal, var newValue, const std::string& varName = "");
	void commandUndo() override;
	void commandRedo() override;

	bool        merge(std::shared_ptr<BaseCommand>& ptr) override;
	std::string getDescription() const override;

private:
	std::string m_VariableName = "UnknownVariable";
	var*        m_object;
	var         m_OldValue;
	var         m_NewValue;
};

template <typename var>
bool PointerVarChanged<var>::merge(std::shared_ptr<BaseCommand>& ptr)
{
	if (!BaseCommand::merge(ptr))
	{
		return false;
	}

	if (auto otherCommand = std::dynamic_pointer_cast<PointerVarChanged<var>, BaseCommand>(ptr))
	{
		const bool sameAddress = m_object == otherCommand->m_object;

		if (sameAddress)
		{
			otherCommand->m_NewValue = m_NewValue;
			return true;
		}
	}

	return false;
}

template <typename var>
PointerVarChanged<var>::PointerVarChanged(var*               value, const var oldVal, const var newValue,
                                          const std::string& varName)
	: m_VariableName(varName), m_object(value)
{
	if (!m_object)
	{
		LOGGER.Err("Could not find variable");
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

	m_OldValue = oldVal;
	m_NewValue = newValue;
}

template <typename var>
void PointerVarChanged<var>::commandUndo()
{
	if (!m_object)
	{
		LOGGER.Warn("Could not find component to undo");
		return;
	}

	*m_object = m_OldValue;
}

template <typename var>
void PointerVarChanged<var>::commandRedo()
{
	if (!m_object)
	{
		LOGGER.Warn("Could not find component to redo");
		return;
	}
	*m_object = m_NewValue;
}

template <typename var>
std::string PointerVarChanged<var>::getDescription() const
{
	if constexpr (refl::trait::is_reflectable_v<var> && refl::trait::is_container_v<var> &&
		refl::runtime::detail::is_ostream_printable_v<char, var>)
	{
		const std::string outMessage =
			std::format("{}:{} changed from {} to {}", refl::reflect<var>().name.str(), m_VariableName,
			            refl::runtime::debug_str(m_OldValue, true), refl::runtime::debug_str(m_NewValue, true));

		return outMessage;
	}
	else
	{
		// if (refl::is_reflectable<var>())
		//{
		//     return std::format("{}:{} changed.", refl::reflect<var>().name.str(), m_VariableName);
		// }
		// else
		{
			return std::format("{}:{} changed.", "UnknownVariable", m_VariableName);
		}
	}
}
#pragma endregion

#pragma region varComponentChange
template <typename ComponentType, typename var>
class VarChanged : public BaseCommand
{
public:
	VarChanged(ComponentType*     object, const var* value, var oldVal, var newValue,
	           const std::string& varName = "");
	void commandUndo() override;
	void commandRedo() override;

	bool        merge(std::shared_ptr<BaseCommand>& ptr) override;
	std::string getDescription() const override;

private:
	std::string      m_VariableName = "UnknownVariable";
	const GameObject m_object;
	size_t           addressOffset;
	var              m_OldValue;
	var              m_NewValue;
};

template <typename ComponentType, typename var>
bool VarChanged<ComponentType, var>::merge(std::shared_ptr<BaseCommand>& ptr)
{
	if (!BaseCommand::merge(ptr))
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
VarChanged<ComponentType, var>::VarChanged(ComponentType* object, const var*           value, const var oldVal,
                                           const var      newValue, const std::string& varName)
	: m_VariableName(varName), m_object(object->gameObject())
{
	if (!object)
	{
		LOGGER.Err("Could not find component");
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

	addressOffset = reinterpret_cast<const char*>(value) - reinterpret_cast<char*>(&*object);
	m_OldValue = oldVal;
	m_NewValue = newValue;
}

template <typename ComponentType, typename var>
void VarChanged<ComponentType, var>::commandUndo()
{
	Component* component = m_object.TryGetComponent<ComponentType>();
	if (!component)
	{
		LOGGER.Warn("Could not find component to undo");
		return;
	}

	const auto& address = reinterpret_cast<char*>(component);
	*reinterpret_cast<var*>(address + addressOffset) = m_OldValue;
}

template <typename ComponentType, typename var>
void VarChanged<ComponentType, var>::commandRedo()
{
	Component* component = m_object.TryGetComponent<ComponentType>();
	if (!component)
	{
		LOGGER.Warn("Could not find component to redo");
		return;
	}
	const auto& address = reinterpret_cast<char*>(component);
	*reinterpret_cast<var*>(address + addressOffset) = m_NewValue;
}

template <typename ComponentType, typename var>
std::string VarChanged<ComponentType, var>::getDescription() const
{
	if constexpr (refl::trait::is_reflectable_v<var> || refl::trait::is_container_v<var> ||
		refl::runtime::detail::is_ostream_printable_v<char, var>)
	{
		const std::string outMessage =
			std::format("{}:{} changed from {} to {}", refl::reflect<ComponentType>().name.str(), m_VariableName,
			            refl::runtime::debug_str(m_OldValue, true), refl::runtime::debug_str(m_NewValue, true));

		return outMessage;
	}
	else
	{
		const std::string outMessage =
			std::format("{}:{} changed.", refl::reflect<ComponentType>().name.str(), m_VariableName);
		return outMessage;
	}
}

#pragma endregion
