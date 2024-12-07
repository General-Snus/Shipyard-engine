#include <functional>
#include "../Event.h"

void Event::RemoveListener(const callback& func)
{
	for (size_t i = 0; i < m_CallbackFunctions.size(); ++i)
	{  
		if (&m_CallbackFunctions[i] == &func)
		{
			m_CallbackFunctions[i] = m_CallbackFunctions.back();
			m_CallbackFunctions.pop_back();
			break;
		}		
	}
}

void Event::Invoke() const
{
	for (auto& callback : m_CallbackFunctions)
	{
		callback();
	}
}
