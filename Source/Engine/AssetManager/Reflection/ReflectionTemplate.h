#pragma once
#include <ranges>
#include <type_traits>

#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/ImGui/ImGui/imgui_internal.h"
#include "Tools/ImGui/ImGui/misc/cpp/imgui-combo-filter.h"

#include "Tools/Reflection/refl.hpp"
#include "Tools/Utilities/LinearAlgebra/Vectors.hpp"
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Editor/Editor/Commands/VarChanged.h> 
#include <Tools/Utilities/TemplateHelpers.h>

class AssetBase;
// TODO refactor out shit from this file


namespace Reflection
{
template <typename T> bool ImGuiReflect(T &ref, const std::string &identifier)
{
    identifier;
    ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
    return false;
}

template <typename T> bool ImGuiReflect(const T &ref, const std::string &identifier)
{
    identifier;
    ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
    return false;
}

inline bool ImGuiReflect(bool &ref, const std::string &identifier)
{
    return ImGui::Checkbox(std::format("##{}", identifier).c_str(), &ref);
}

inline bool ImGuiReflect(Color &ref, const std::string &identifier)
{
    bool returnArg = false;
    // Show selection button of all active blends,
    // if address of ref have a corresponding blend or mix, use that one,

    static std::vector<std::string> keys;
    keys.reserve(ColorManager::m_NamedColor.size());
    keys.clear();
    for (const auto &key : ColorManager::m_NamedColor | std::ranges::views::keys)
    {
        keys.push_back(key);
    }
    ImGui::Separator();
    ImGui::Text("Color preset menu");
    static bool blendColor = false;
    returnArg |= ImGui::Checkbox("Use blended color:", &blendColor);

    if (blendColor)
    {
        bool changed = false;
        static int selected_item1 = -1;
        changed |= ImGui::ComboAutoSelect("Blend value 1", selected_item1, keys, item_getter1, autoselect_search_vector,
                                          ImGuiComboFlags_HeightSmall);

        static int selected_item2 = -1;
        changed |= ImGui::ComboAutoSelect("Blend value 2", selected_item2, keys, item_getter1, autoselect_search_vector,
                                          ImGuiComboFlags_HeightSmall);
        static float blendFactor = 0.5f;
        changed |= ImGui::SliderFloat("Blend", &blendFactor, 0.0f, 1.0f);
        ImGui::SameLine();

        Vector4f blend1 = selected_item1 == -1 ? Vector4f(1, 1, 1, 1) : ColorManager::GetColor(keys.at(selected_item1));
        Vector4f blend2 = selected_item2 == -1 ? Vector4f(1, 1, 1, 1) : ColorManager::GetColor(keys.at(selected_item2));

        Vector4f color = Color::RGBLerp(blend1, blend2, blendFactor);
        if (changed)
        {
            ref.SetColor(color);
        }
        ImGui::ColorButton("Color:", color);
        returnArg = changed;
    }
    else
    {
        int preSelected = static_cast<int>(
            std::distance(ColorManager::m_NamedColor.begin(), ColorManager::m_NamedColor.find(ref.m_ColorName)));

        static int selected_item1 = -1;

        selected_item1 = preSelected != ColorManager::m_NamedColor.size() ? preSelected : -1;

        if (ImGui::ComboAutoSelect("Preset", selected_item1, keys, item_getter1, autoselect_search_vector,
                                   ImGuiComboFlags_HeightSmall) &&
            selected_item1 != -1)
        {
            returnArg = true;
            ref.m_ColorName = keys.at(selected_item1);
        }
        auto colorArray = ref.GetRGBA();
        if (ImGui::ColorEdit4(std::format("##{}", identifier).c_str(), &colorArray.x, ImGuiColorEditFlags_NoInputs))
        {
            returnArg = true;
            ref.SetColor(colorArray);
            selected_item1 = -1;
        }
    }
    return returnArg;
}

inline bool ImGuiReflect(Vector2<float> &ref, const std::string &identifier)
{
    return ImGui::DragFloat2(std::format("##{}", identifier).c_str(), &ref.x);
}

inline bool ImGuiReflect(Vector3<float> &ref, const std::string &identifier)
{
    return ImGui::DragFloat3(std::format("##{}", identifier).c_str(), &ref.x);
}

inline bool ImGuiReflect(Vector4<float> &ref, const std::string &identifier)
{
    return ImGui::DragFloat4(std::format("##{}", identifier).c_str(), &ref.x);
}

inline bool ImGuiReflect(float &ref, const std::string &identifier)
{
    return ImGui::DragFloat(std::format("##{}", identifier).c_str(), &ref);
}

template <typename T>
concept pointerSyntax = requires(T a) { a->GetTypeInfo(); };

template <typename T>
concept IsComponent = requires(T a) { a.GetGameObject(); };
template <typename T>
concept IsComponentPtr = requires(T a) { a->GetGameObject(); };

template <typename T>
concept InspectorSyntax = requires(T a) { a.InspectorView(); };
template <typename T>
concept InspectorSyntaxPtr = requires(T a) { a->InspectorView(); };
} // namespace Reflection
struct serializable : refl::attr::usage::field, refl::attr::usage::function
{
};
template <typename T> void serialize(std::ostream &os, T &&value)
{
    // iterate over the members of T
    for_each(refl::reflect(value).members, [&](auto member) {
        // is_readable checks if the member is a non-const field
        // or a 0-arg const-qualified function marked with property attribute
        if constexpr (is_readable(member) && refl::descriptor::has_attribute<serializable>(member))
        {
            // get_display_name prefers the friendly_name of the property over the function name
            os << get_display_name(member) << "=";
            // member(value) returns a reference to the field or calls the property accessor
            os << member(value) << ";";
        }
    });
}

class TypeInfo
{
  public:
    // instances can be obtained only through calls to Get()
    template <typename T> static const TypeInfo &Get()
    {
        // here we create the singleton instance for this particular type
        static const TypeInfo ti(refl::reflect<T>());
        return ti;
    }

    const std::string &Name() const
    {
        return name_;
    }

    const std::type_info *TypeID() const
    {
        return typeId;
    }

  private:
    // were only storing the name for demonstration purposes,
    // but this can be extended to hold other properties as well
    std::string name_;
    const std::type_info *typeId;

    // given a type_descriptor, we construct a TypeInfo
    // with all the metadata we care about (currently only name)
    template <typename T, typename... Fields> TypeInfo(refl::type_descriptor<T> td) : name_(td.name), typeId(&typeid(T))
    {
    }
};

template <typename T, typename Type = std::remove_cvref_t<T>>
inline constexpr const bool isStdType = get_name(refl::reflect<Type>()).template substr<0, 5>() == "std::";

template <class T, typename RawType = std::remove_cvref_t<T>> constexpr bool isReflectableClass()
{
    if constexpr (std::is_class_v<RawType> && refl::is_reflectable<RawType>() && !std::is_fundamental_v<RawType> &&
                  !std::is_array_v<RawType>)
    {
        return !isStdType<RawType>;
    }
    return false;
}

template <typename T> constexpr bool isReflectableClass_v = isReflectableClass<T>();
template <class T>
concept ReflectableClass = isReflectableClass<T>();

class Reflectable
{
  public:
    virtual const TypeInfo &GetTypeInfo() const = 0;
    virtual bool InspectorView()
    {
        return false;
    };

    template <typename T0 = Component> void Reflect(this auto &aReflectedObject);
    template <typename T0 = Component> void Reflect(T0 &aReflectedObject);
    template <typename T0, typename C> static void UpdateValue(T0 &member, C &aReflectedObject, const std::string &arg);
    template <typename T0, typename C>
    static void UpdateCleanValue(T0 &member, C &aReflectedObject, const std::string &arg);
};

#define MYLIB_REFLECTABLE()                                                                                            \
    virtual const TypeInfo &GetTypeInfo() const override                                                               \
    {                                                                                                                  \
        return TypeInfo::Get<::refl::trait::remove_qualifiers_t<decltype(*this)>>();                                   \
    }

#pragma warning(push)
#pragma warning(disable : 4702)

// Reflect public reflected members
template <typename T0> // Make sure only reflected objects can get here
inline void Reflectable::Reflect(this auto &aReflectedObject)
{
    if constexpr (!isReflectableClass<T0>())
    {
        return;
    }

    auto imp = [&]<typename T1 = int>(T1 member) {
        using declType = decltype(member(aReflectedObject));
        if constexpr (Reflection::InspectorSyntaxPtr<declType>)
        {
            member(aReflectedObject)->InspectorView();
            return;
        }
        if constexpr (Reflection::InspectorSyntax<declType>)
        {
            member(aReflectedObject).InspectorView();
            return;
        }

        const std::string arg = std::string(get_display_name(member)) + ": ";

        ImGui::PushID(arg.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 200);
        ImGui::Text(arg.data());
        ImGui::NextColumn();
        UpdateValue(member, aReflectedObject, arg);

        ImGui::Columns(1);
        ImGui::PopID();
        ImGui::Indent(16.f);
        ImGui::Unindent(16.0f);
    };
    refl::util::for_each(refl::reflect(aReflectedObject).members, imp);
    ImGui::Separator();
}
template <typename T0, typename C>
inline void Reflectable::UpdateValue(T0 &member, C &aReflectedObject, const std::string &arg)
{
    using MemberType = std::remove_reference_t<decltype(unwrapPointer(member(aReflectedObject)))>;
    using componentType = std::remove_reference_t<decltype(unwrapPointer(aReflectedObject))>;
    using declType = decltype(member(aReflectedObject));

    using ptrType = std::add_pointer_t<MemberType>;
    using cmpType = std::add_pointer_t<T0>;

    const auto oldValue = member(aReflectedObject);
    const bool changed = Reflection::ImGuiReflect(member(aReflectedObject), arg);

    constexpr bool isConvertible = std::is_convertible_v<cmpType, Component *>;
    constexpr bool isBase = std::is_base_of_v<Component, T0>;
    if constexpr (isConvertible && isBase)
    {
        if (changed)
        {
            const auto newValue = member(aReflectedObject);
            if constexpr (SmartPointerType<declType>)
            {
                isConvertible;
                newValue;
                oldValue;
                // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            else if constexpr (std::is_pointer_v<declType>)
            {
                isConvertible;
                newValue;
                oldValue;
                // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            else
            {
                const MemberType *unwrapped = &unwrapPointer(member(aReflectedObject));
                componentType *unwrappedComponent = &(aReflectedObject);
                const MemberType old = oldValue;
                const MemberType neww = newValue;

                const auto ptr = std::make_shared<VarChanged<componentType, MemberType>>(unwrappedComponent, unwrapped,
                                                                                         old, neww, arg);
                CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
            }
        }
    }
    else
    {

        if constexpr (!isReflectableClass<MemberType>())
        {
            if (changed)
            {
                const auto newValue = member(aReflectedObject);
                if constexpr (SmartPointerType<declType>)
                {
                    newValue;
                    oldValue;
                    // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                    //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                    // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                else if constexpr (std::is_pointer_v<declType>)
                {
                    newValue;
                    oldValue;
                    // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                    //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                    // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                else
                {
                    const auto ptr = std::make_shared<PointerVarChanged<MemberType>>(
                        &unwrapPointer(member(aReflectedObject)), oldValue, newValue, arg);
                    CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
                }
            }
        }
    }
}
template <typename T0, typename C>
inline void Reflectable::UpdateCleanValue(T0 &member, C &aReflectedObject, const std::string &arg)
{
    using MemberType = std::remove_reference_t<decltype(unwrapPointer(member))>;
    using componentType = std::remove_reference_t<decltype(unwrapPointer(aReflectedObject))>;
    using declType = decltype(member);

    using ptrType = std::add_pointer_t<MemberType>;
    using cmpType = std::add_pointer_t<T0>;

    const auto oldValue = member;
    const bool changed = Reflection::ImGuiReflect(member, arg);

    constexpr bool isConvertible = std::is_convertible_v<cmpType, Component *>;
    constexpr bool isBase = std::is_base_of_v<Component, T0>;
    if constexpr (isConvertible && isBase)
    {
        if (changed)
        {
            const auto newValue = member;
            if constexpr (SmartPointerType<declType>)
            {
                isConvertible;
                newValue;
                oldValue;
                // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            else if constexpr (std::is_pointer_v<declType>)
            {
                isConvertible;
                newValue;
                oldValue;
                // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            else
            {
                const MemberType *unwrapped = &unwrapPointer(member);
                componentType *unwrappedComponent = &(aReflectedObject);
                const MemberType old = oldValue;
                const MemberType neww = newValue;

                const auto ptr = std::make_shared<VarChanged<componentType, MemberType>>(unwrappedComponent, unwrapped,
                                                                                         old, neww, arg);
                CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
            }
            if (ImGui::IsItemDeactivatedAfterEdit())
            {
                CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
            }
        }
    }
    else
    {

        if constexpr (!isReflectableClass<MemberType>())
        {
            if (changed)
            {
                const auto newValue = member;
                if constexpr (SmartPointerType<declType>)
                {
                    newValue;
                    oldValue;
                    // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                    //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                    // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                else if constexpr (std::is_pointer_v<declType>)
                {
                    newValue;
                    oldValue;
                    // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
                    //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
                    // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                else
                {
                    const auto ptr = std::make_shared<PointerVarChanged<MemberType>>(&unwrapPointer(member), oldValue,
                                                                                     newValue, arg);
                    CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
                }
                if (ImGui::IsItemDeactivatedAfterEdit())
                {
                    CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
                }
            }
        }
    }
};

template <typename T0> __forceinline void Reflectable::Reflect(T0 &aReflectedObject)
{
    if constexpr (!isReflectableClass<T0>())
    {
        return;
    }

    auto imp = [&]<typename T1 = float>(const T1 member) {
        using MemberType = std::remove_reference_t<decltype(unwrapPointer(member(aReflectedObject)))>;
        using declType = decltype(member(aReflectedObject));

        if constexpr (Reflection::InspectorSyntaxPtr<declType>)
        {
            member(aReflectedObject)->InspectorView();
            return;
        }
        if constexpr (Reflection::InspectorSyntax<declType>)
        {
            member(aReflectedObject).InspectorView();
            return;
        }

        const std::string arg = std::string(get_display_name(member)) + ": ";

        ImGui::PushID(arg.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 200);
        ImGui::Text(arg.data());
        ImGui::NextColumn();

        UpdateValue(member, aReflectedObject, arg);
        // const auto oldValue = member(aReflectedObject);
        // const bool changed = Reflection::ImGuiReflect(member(aReflectedObject), arg);
        //
        // if constexpr (!isReflectableClass<MemberType>())
        //{
        //     if (changed)
        //     {
        //         const auto newValue = member(aReflectedObject);
        //         if constexpr (SmartPointerType<declType>)
        //         {
        //             newValue;
        //             oldValue;
        //             // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
        //             //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
        //             // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
        //         }
        //         else if constexpr (std::is_pointer_v<declType>)
        //         {
        //             newValue;
        //             oldValue;
        //             // const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
        //             //     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
        //             // CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
        //         }
        //         else
        //         {
        //             const auto ptr = std::make_shared<PointerVarChanged<MemberType>>(
        //                 &unwrapPointer(member(aReflectedObject)), oldValue, newValue, arg);
        //             CommandBuffer::MainEditorCommandBuffer().AddCommand(ptr);
        //         }
        //         if (ImGui::IsItemDeactivatedAfterEdit())
        //         {
        //             CommandBuffer::MainEditorCommandBuffer().GetLastCommand()->SetMergeBlocker(true);
        //         }
        //     }
        // }

        ImGui::Columns(1);
        ImGui::PopID();
        ImGui::Indent(16.f);
        ImGui::Unindent(16.0f);
    };
    refl::util::for_each(refl::reflect(aReflectedObject).members, imp);
    ImGui::Separator();
}
#pragma warning(pop)
