#pragma once
#include "CommandBuffer.h"
#include <memory>


class Component;


template<typename VariableType>
class VarChanged : public BaseCommand
{
public:
	VarChanged(const std::shared_ptr<Component>& comp,VariableType* value, const VariableType oldVal, const VariableType newValue);
	void Undo() override;
	void Do() override;


private: 
	std::shared_ptr<Component> m_Component;
	size_t addressOffset; 
	VariableType m_OldValue;
	VariableType m_NewValue;
};

template<typename VariableType>
inline VarChanged<VariableType>::VarChanged(const std::shared_ptr<Component>& comp, VariableType* value, const VariableType oldVal, const VariableType newValue)
{
	m_Component = comp;
	addressOffset = reinterpret_cast<char*>(value) - reinterpret_cast<char*>(&*comp);
	m_OldValue = oldVal;
	m_NewValue = newValue;

}

template<typename VariableType>
inline void VarChanged<VariableType>::Undo()
{
	const auto& address = reinterpret_cast<char*>(&*m_Component);
	*reinterpret_cast<VariableType*>(address + addressOffset) = m_OldValue;
}

template<typename VariableType>
inline void VarChanged<VariableType>::Do()
{
	const auto& address = reinterpret_cast<char*>(&*m_Component);
	*reinterpret_cast<VariableType*>(address + addressOffset) = m_NewValue;
}
