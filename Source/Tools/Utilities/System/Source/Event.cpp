#include <functional>
#include "../Event.h"

void Event::RemoveListener(callback& func)
{
	func;
}

void Event::Invoke()
{
	for (auto& callback : m_CallbackFunctions)
	{
		callback();
	}
}
