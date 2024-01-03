/*****************************************************************************
 * This file contains types that ScriptGraph requires to function by default.
 * You can register your own types here or using a separate header file in
 * your final project. See the RegisterExternalTypes.h example file.
 *****************************************************************************/

#pragma once

#ifdef WITH_EDITOR
#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"
#endif

#include "ScriptGraph/ScriptGraphTypes.h"

BeginDataTypeHandler(Float, float, GraphColor(156, 246, 60, 255), true)

#ifdef WITH_EDITOR
void RenderConstructWidget(const std::string& aContainerUUID, void* aDataPtr, const ScriptGraphType& aTypeInfo) override
{
	const float y = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(y - 2);
	const ImVec2 inputSize = ImGui::CalcTextSize("0.0000");
	ImGui::SetNextItemWidth(inputSize.x);
	ImGui::InputFloat(aContainerUUID.c_str(), static_cast<float*>(aDataPtr), 0, 0, "%.1f");
}
#endif

std::string ToString(const void* aDataPtr, const ScriptGraphType& aTypeInfo) const override
{
	aTypeInfo;
	return std::to_string(*static_cast<const float*>(aDataPtr));
}
EndDataTypeHandler

BeginDataTypeHandler(Bool, bool, GraphColor(149, 0, 0, 255), true)

#ifdef WITH_EDITOR
void RenderConstructWidget(const std::string& aContainerUUID, void* aDataPtr, const ScriptGraphType& aTypeInfo) override
{
	const float y = ImGui::GetCursorPosY(); ImGui::SetCursorPosY(y - 2); ImGui::Checkbox(aContainerUUID.c_str(), static_cast<bool*>(aDataPtr));
}
#endif

std::string ToString(const void* aDataPtr, const ScriptGraphType& aTypeInfo) const override
{
	aTypeInfo;
	return *static_cast<const bool*>(aDataPtr) ? "True" : "False";
}
EndDataTypeHandler

// Example type with custom de/serializer.
BeginDataTypeHandler(String, std::string, GraphColor(250, 0, 208, 255), true)

#ifdef WITH_EDITOR
void RenderConstructWidget(const std::string& aContainerUUID, void* aDataPtr, const ScriptGraphType& aTypeInfo) override
{
	ImGui::PushItemWidth(250);
	ImGui::InputText(aContainerUUID.c_str(), static_cast<std::string*>(aDataPtr));
	ImGui::PopItemWidth();	
}
#endif

std::string ToString(const void* aDataPtr, const ScriptGraphType& aTypeInfo) const override
{
	aTypeInfo;
	return *static_cast<const std::string*>(aDataPtr);
}

void SerializeData(const void* aDataPtr, const ScriptGraphType& aTypeInfo, std::vector<uint8_t>& outData) const override
{
	aTypeInfo;
	// Strings are "fun" :P
	// We assume that aDataPtr has a null terminator at the end...
	const std::string* aString = static_cast<const std::string*>(aDataPtr);

	// We can't trust the size in typeinfo for the size of a string data block.
	// Since we're not wstring we're just string length * sizeof(char) which is string length.
	outData.resize(aString->length());
	memcpy_s(outData.data(), outData.capacity(), aString->c_str(), aString->length());
}
void DeserializeData(const std::vector<uint8_t>& inData, const ScriptGraphType& aTypeInfo, void* outDataPtr) const override
{
	aTypeInfo;
	// And when we build a string back from a vector we need to make sure it's sized properly.
	// outDataPtr is already initialized as a pointer of the correct type.
	if (!inData.empty())
	{
		std::string* aString = static_cast<std::string*>(outDataPtr);
		const std::string dataString = std::string(inData.begin(), inData.end());
		*aString = dataString;
	} 
}
EndDataTypeHandler

BeginDataTypeHandler(Integer, int, GraphColor(14, 224, 161, 255), true)

#ifdef WITH_EDITOR
void RenderConstructWidget(const std::string& aContainerUUID, void* aDataPtr, const ScriptGraphType& aTypeInfo) override
{
	const float y = ImGui::GetCursorPosY();
	ImGui::SetCursorPosY(y - 2);
	const ImVec2 inputSize = ImGui::CalcTextSize("1000");
	ImGui::SetNextItemWidth(inputSize.x);
	ImGui::InputInt(aContainerUUID.c_str(), static_cast<int*>(aDataPtr), 0, 0);
}
#endif

std::string ToString(const void* aDataPtr, const ScriptGraphType& aTypeInfo) const override
{
	aTypeInfo;
	return std::to_string(*static_cast<const int*>(aDataPtr));
}
EndDataTypeHandler