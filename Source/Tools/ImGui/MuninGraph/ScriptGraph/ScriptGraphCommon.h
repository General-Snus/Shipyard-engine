#pragma once
#include <type_traits>
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

enum class NodeResultState : unsigned
{
	Error = 0,
	InProgress,
	Finished,
};

struct NodeResult
{
	NodeResult(NodeResultState aState, size_t aValue)
		: myState(aState), myValue(aValue)
	{  }

	NodeResult()
		: myState(NodeResultState::Error), myValue(0)
	{  }

	FORCEINLINE NodeResultState State() const { return myState; }
	FORCEINLINE size_t Data() const { return myValue; }

private:
	NodeResultState myState;
	size_t myValue;
};

enum class NodeEntryType : unsigned
{
	New,
	InProgress
};

enum class ScriptGraphPinIcon
{
	Exec,
	Circle,
	Square,
	Array,
	RoundedSquare,
	Diamond
};

enum class ScriptGraphPinType : unsigned
{
	Exec,
	Data,
	Variable
};

enum class ScriptGraphNodeType : unsigned
{
	Function,
	Event,
	Variable
};

enum ScriptGraphVariableFlags_
{
	ScriptGraphVariableFlag_None		= 0,
	ScriptGraphVariableFlag_ReadOnly	= 1 << 1,
	ScriptGraphVariableFlag_Internal	= 1 << 2,
};

typedef int ScriptGraphVariableFlags;

template<typename E>
constexpr auto EnumAsIntegral(E e) -> std::underlying_type_t<E>
{
   return static_cast<std::underlying_type_t<E>>(e);
}