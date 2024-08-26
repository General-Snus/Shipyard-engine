#include "pch.h"

#include "ScriptGraphEditorTypes.h"
#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/ImGui/ImGui/misc/cpp/imgui_stdlib.h"
#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include <Engine/AssetManager/ComponentSystem/GameObject.h>

extern "C" void __MuninGraph_AutoRegEditorTypes()
{
}

IMPLEMENT_EDITOR_TYPE(float, Float)

bool ScriptGraphEditorType_Float::TypeEditWidget(std::string_view aUniqueName,
                                                 const TypedDataContainer &aDataContainer) const
{
    const float y = ImGui::GetCursorPosY();
    ImGui::SetCursorPosY(y - 2);
    const ImVec2 inputSize = ImGui::CalcTextSize("0.0000");
    ImGui::SetNextItemWidth(inputSize.x);
    ImGui::InputFloat(aUniqueName.data(), static_cast<float *>(*aDataContainer), 0, 0, "%.1f");
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        return true;
    }

    return false;
}

std::string ScriptGraphEditorType_Float::ToString(const TypedDataContainer &aDataContainer) const
{
    float f = 0;
    aDataContainer.TryGet(f);
    return std::to_string(f);
};

IMPLEMENT_EDITOR_TYPE(std::string, String)

bool ScriptGraphEditorType_String::TypeEditWidget(std::string_view aUniqueName,
                                                  const TypedDataContainer &aDataContainer) const
{
    ImGui::NewLine();
    ImGui::PushItemWidth(150);
    ImGui::InputText(aUniqueName.data(), static_cast<std::string *>(*aDataContainer),
                     ImGuiInputTextFlags_EnterReturnsTrue);
    bool result = false;
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        result = true;
    }
    ImGui::PopItemWidth();
    return result;
}

std::string ScriptGraphEditorType_String::ToString(const TypedDataContainer &aDataContainer) const
{
    return *static_cast<const std::string *>(*aDataContainer);
}

static AutoRegisterEditorType<ScriptGraphEditorType_Internal_VoidPtr> zz__Internal_VoidPtrEditorTypeRegistration;
ScriptGraphEditorType_Internal_VoidPtr::ScriptGraphEditorType_Internal_VoidPtr()
    : ScriptGraphEditorType(TypeRegistry::Get().Resolve(typeid(void *))) {};

// Bool type
IMPLEMENT_EDITOR_TYPE(bool, Bool)

bool ScriptGraphEditorType_Bool::TypeEditWidget(std::string_view aUniqueName,
                                                const TypedDataContainer &aDataContainer) const
{
    bool value = *static_cast<bool *>(*aDataContainer);
    if (ImGui::Checkbox(aUniqueName.data(), &value))
    {
        *static_cast<bool *>(*aDataContainer) = value;
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_Bool::ToString(const TypedDataContainer &aDataContainer) const
{
    bool value = *static_cast<const bool *>(*aDataContainer);
    return value ? "true" : "false";
}

// Size_t type
IMPLEMENT_EDITOR_TYPE(size_t, Size_t)

bool ScriptGraphEditorType_Size_t::TypeEditWidget(std::string_view aUniqueName,
                                                  const TypedDataContainer &aDataContainer) const
{
    size_t value = *static_cast<size_t *>(*aDataContainer);
    if (ImGui::InputScalar(aUniqueName.data(), ImGuiDataType_U64, &value))
    {
        *static_cast<size_t *>(*aDataContainer) = value;
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_Size_t::ToString(const TypedDataContainer &aDataContainer) const
{
    size_t value = *static_cast<const size_t *>(*aDataContainer);
    return std::to_string(value);
}

// Int type
IMPLEMENT_EDITOR_TYPE(int, Int)

bool ScriptGraphEditorType_Int::TypeEditWidget(std::string_view aUniqueName,
                                               const TypedDataContainer &aDataContainer) const
{
    int value = *static_cast<int *>(*aDataContainer);
    if (ImGui::InputInt(aUniqueName.data(), &value))
    {
        *static_cast<int *>(*aDataContainer) = value;
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_Int::ToString(const TypedDataContainer &aDataContainer) const
{
    int value = *static_cast<const int *>(*aDataContainer);
    return std::to_string(value);
}

// Vector3f type
IMPLEMENT_EDITOR_TYPE(Vector3f, Vector3)

bool ScriptGraphEditorType_Vector3::TypeEditWidget(std::string_view aUniqueName,
                                                   const TypedDataContainer &aDataContainer) const
{
    Vector3f value = *static_cast<Vector3f *>(*aDataContainer);
    if (ImGui::InputFloat3(aUniqueName.data(), &value.x))
    {
        *static_cast<Vector3f *>(*aDataContainer) = value;
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_Vector3::ToString(const TypedDataContainer &aDataContainer) const
{
    Vector3f value = *static_cast<const Vector3f *>(*aDataContainer);
    return "{" + std::to_string(value.x) + ", " + std::to_string(value.y) + ", " + std::to_string(value.z) + "}";
}

// GameObject type
IMPLEMENT_EDITOR_TYPE(GameObject, GameObject)

bool ScriptGraphEditorType_GameObject::TypeEditWidget(std::string_view aUniqueName,
                                                      const TypedDataContainer &aDataContainer) const
{
    // Assuming GameObject has a method to get its name or ID for display
    GameObject value = *static_cast<GameObject *>(*aDataContainer);

    if (!value.IsValid())
    {
        return false;
    }

    std::string name = value.GetName();
    if (ImGui::InputText(aUniqueName.data(), &name, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_GameObject::ToString(const TypedDataContainer &aDataContainer) const
{
    GameObject value = *static_cast<const GameObject *>(*aDataContainer);
    return value.GetName(); // Replace with actual method to get name or ID
}

// SY::UUID type
IMPLEMENT_EDITOR_TYPE(SY::UUID, UUID)

bool ScriptGraphEditorType_UUID::TypeEditWidget(std::string_view aUniqueName,
                                                const TypedDataContainer &aDataContainer) const
{
    SY::UUID value = *static_cast<SY::UUID *>(*aDataContainer);
    std::string uuidStr = std::to_string(value); // Replace with actual method to convert UUID to string
    if (ImGui::InputText(aUniqueName.data(), &uuidStr, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        // Update UUID with new value if necessary
        return true;
    }
    return false;
}

std::string ScriptGraphEditorType_UUID::ToString(const TypedDataContainer &aDataContainer) const
{
    SY::UUID value = *static_cast<const SY::UUID *>(*aDataContainer);
    return std::to_string(value);
}
