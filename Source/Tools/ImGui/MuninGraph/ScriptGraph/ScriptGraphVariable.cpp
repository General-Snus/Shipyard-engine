#include "ScriptGraphVariable.h"

void ScriptGraphVariable::Reset() const
{
	std::vector<uint8_t> temp;
	Default.Serialize(temp);
	Data.Deserialize(temp);
}

void ScriptGraphVariable::SetFlags(int aFlags)
{
	myFlags |= aFlags;
}
