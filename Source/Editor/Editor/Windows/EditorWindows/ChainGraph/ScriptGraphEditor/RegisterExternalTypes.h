#pragma once
// Place your includes for registered types here.
#include <cinttypes>

#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <ScriptGraph/ScriptGraphTypes.h>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>
#include <format>
/********************************************************************************************************************************
 *
 * Here you should declare all types that the ScriptGraph should be aware of. I.e. all
 * types that can appear as a Pin on a Script Graph Node or as a Variable.
 *
 * Format goes:
 * BeginDataTypeHandler(...)
 * <specific code or nothing at all>
 * EndDataTypeHandler
 *
 * BeginDataTypeHandler(FriendlyName, Type, Color, InPlaceConstructible)
 * - FriendlyName is the name you will see in the UI for this type.
 * - Type is the C++ type itself, like float, std::string, GameObject* etc.
 * - Color is the color which will be used to identify this type of variable.
 * - InPlaceConstructible is if we can create this type with a ImGui widget or not
 *   during runtime. You MUST override RenderconstructWidget between Begin and End for
 *	 this to work. See examples below.
 *
 * You can override methods found in the base class:
 * void RenderConstructWidget(const std::string& aContainerUUID, void* aDataPtr, const ScriptGraphTypeHandler&
 aTypeInfo) override
 * - This should draw an ImGui widget to edit the provided data pointer.
 * - Only applies if InPlaceConstructible is True.
 *
 * std::string ToString(const void* aDataPtr, const ScriptGraphTypeHandler& aTypeInfo) const override
 * - This should return a string representation of the data, useful for showing default
 *   values in the UI or other things. Allows calling of ScriptGraphDataTypeRegistry::GetString
 *   to get an easily loggable representation of the contents in a data container.
 *
 * virtual void SerializeData(const void* aDataPtr, const ScriptGraphTypeHandler& aTypeInfo, std::vector<uint8_t>&
 outData) const
 * - If, and only if, you need to handle serialization of your data type on your own. The default serializer works for
 all
 *   POD types, but if you want to serialize pointers and things you need to do this yourself :).
 *
 * virtual void DeserializeData(const std::vector<uint8_t>& inData, const ScriptGraphTypeHandler& aTypeInfo, void*
 outDataPtr) const
  - If, and only if, you need to handle deserialization of your data type on your own. The default deserializer works
 for all
 *  POD types, but if you want to deserialize pointers and things you need to do this yourself :).
 *
 *******************************************************************************************************************************/

BeginDataTypeHandler(Vector, Vector3f, GraphColor(255, 200, 0, 255), false)

    std::string ToString(const void *aDataPtr, const ScriptGraphType &aTypeInfo) const override
{
    aTypeInfo;
    const Vector3f vector = *static_cast<const Vector3f *>(aDataPtr);
    std::string result = std::format("X: {}, Y: {}, Z: {}", vector.x, vector.y, vector.z);
    return result;
}
EndDataTypeHandler

BeginDataTypeHandler(UniqueID, SY::UUID, GraphColor(155, 155, 0, 255), false)

    void RenderConstructWidget(const std::string &aContainerUUID, void *aDataPtr,
                               const ScriptGraphType &aTypeInfo) override
{
    aTypeInfo;
    const float y = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(y - 2);
    const ImVec2 inputSize = ImGui::CalcTextSize("1000");
    ImGui::SetNextItemWidth(inputSize.x);
    ImGui::InputScalar(aContainerUUID.c_str(), ImGuiDataType_U32, aDataPtr);
}
std::string ToString(const void *aDataPtr, const ScriptGraphType &aTypeInfo) const override
{
    aTypeInfo;
    const SY::UUID id = *static_cast<const SY::UUID *>(aDataPtr);
    std::string result = id;
    return result;
}
EndDataTypeHandler

BeginDataTypeHandler(GameObjects, GameObject, GraphColor(150, 255, 0, 255), false)

    std::string ToString(const void *aDataPtr, const ScriptGraphType &aTypeInfo) const override
{
    aTypeInfo;
    const GameObject go = *static_cast<const GameObject *>(aDataPtr);
    std::string result = std::format("Obj: {}", go.GetName());
    return result;
}
EndDataTypeHandler
