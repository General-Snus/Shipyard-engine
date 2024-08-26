#include "pch.h"
#include "ScriptGraphEditorTypeRegistry.h"

ScriptGraphEditorType::ScriptGraphEditorType(const RegisteredType* aType): myType(aType)
{  }

ScriptGraphEditorType::~ScriptGraphEditorType()
{
	myType = nullptr;
}

bool ScriptGraphEditorType::TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer& aDataContainer) const
{
	return false;
}

std::string ScriptGraphEditorType::ToString([[maybe_unused]] const TypedDataContainer& aDataContainer) const
{
	return "";
}
