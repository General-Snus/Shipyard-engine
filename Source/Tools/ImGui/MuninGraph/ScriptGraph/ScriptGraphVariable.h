#pragma once
#include "ScriptGraphCommon.h"
#include "Types/TypeRegistry.h"

struct ScriptGraphVariable
{
	friend class ScriptGraphSchema;

	TypedDataContainer Data;
	TypedDataContainer Default;
	std::string Name;

	void Reset() const;
	FORCEINLINE const RegisteredType* GetType() const { return Data.GetType(); }
	FORCEINLINE bool HasFlag(ScriptGraphVariableFlags aFlag) const { return (myFlags & aFlag); }
	FORCEINLINE int GetFlags() const { return myFlags; }
	void SetFlags(int aFlags);

private:
	int myFlags = ScriptGraphVariableFlag_None;
};
