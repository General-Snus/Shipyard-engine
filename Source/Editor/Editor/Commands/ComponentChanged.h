#pragma once
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Engine/AssetManager/ComponentSystem/Component.h>

template <typename ComponentType>
class ComponentChanged : public BaseCommand
{
public:
	//Think imgui, i pass identifier, i will merge if previous have same identifier
	ComponentChanged(GameObject changedObject, std::string idenfier);
	~ComponentChanged() = default;

	bool Merge(BaseCommand* ptr)
	{
		ptr;
		return false;
	}

	//The design will force the Component to change twice, first you set it then you save it
	//And adding the command will force the Do, but editor is real time on small scale data so
	//No performance hit
	void Do() override
	{
		m_ChangedObject.GetComponent<ComponentType>() = m_NewValue;
	}

	void commandUndo() override
	{
		m_ChangedObject.GetComponent<ComponentType>() = m_PreviousValue;
	}

	//Additional thoughts,
	/*
	What about volitile member data? TODO THINK ABOUT IT
	
	*/


private:
	GameObject    m_ChangedObject;
	ComponentType m_PreviousValue;
	ComponentType m_NewValue;
};

//template<typename ComponentType>
//inline ComponentChanged<ComponentType>::ComponentChanged(GameObject changedObject) : m_ChangedObject(changedObject)
//{
//
//}

template <typename ComponentType>
ComponentChanged<ComponentType>::ComponentChanged(GameObject changedObject, std::string idenfier)
{
}
