#pragma once

#include "Tools/ImGui/imgui.h"

#include "Reflectable.h"
#include "Tools/Reflection/refl.hpp"
#include "Tools/Utilities/LinearAlgebra/Vectors.hpp"
#include <Editor/Editor/Commands/CommandBuffer.h>
#include <Editor/Editor/Commands/VarChanged.h>
#include <Tools/Utilities/TemplateHelpers.h>

// TODO refactor out shit from this file
class GameObject;

namespace Reflection
{
	template <typename T>
	bool ImGuiReflect(T& ref, const std::string& identifier)
	{
		identifier;
		ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
		return false;
	}

	template <typename T>
	bool ImGuiReflect(const T& ref, const std::string& identifier)
	{
		identifier;
		ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
		return false;
	}

	bool ImGuiReflect(bool& ref, const std::string& identifier);

	bool ImGuiReflect(const GameObject& ref, const std::string& identifier);

	bool ImGuiReflect(Color& ref, const std::string& identifier);

	bool ImGuiReflect(Vector2<float>& ref, const std::string& identifier);

	bool ImGuiReflect(Vector3<float>& ref, const std::string& identifier);

	bool ImGuiReflect(Vector4<float>& ref, const std::string& identifier);

	bool ImGuiReflect(float& ref, const std::string& identifier);

	bool ImGuiReflect(int& ref, const std::string& identifier);

	template <typename T>
	concept InspectorSyntax = requires(T a) { a.InspectorView(); };
	template <typename T>
	concept InspectorSyntaxPtr = requires(T a) { a->InspectorView(); };
	struct serializable : refl::attr::usage::field, refl::attr::usage::function
	{
	};

	template <typename T>
	void serialize(std::ostream& os, T&& value)
	{
		// iterate over the members of T
		for_each(refl::reflect(value).members, [&](auto member)
		{
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



	template <typename T0, typename C>
	bool UpdateValue(T0& member, C& aReflectedObject, const std::string& arg)
	{
		bool changed{};
		using MemberType = std::remove_reference_t<decltype(unwrapPointer(member(aReflectedObject)))>;
		using componentType = std::remove_reference_t<decltype(unwrapPointer(aReflectedObject))>;
		using declType = decltype(member(aReflectedObject));

		using ptrType = std::add_pointer_t<MemberType>;
		using cmpType = std::add_pointer_t<T0>;

		const auto oldValue = member(aReflectedObject);
		changed = Reflection::ImGuiReflect(member(aReflectedObject), arg);

		constexpr bool isConvertible = std::is_convertible_v<cmpType, Component*>;
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
					// CommandBuffer::MainEditorCommandBuffer().addCommand(ptr);
				}
				else if constexpr (std::is_pointer_v<declType>)
				{
					isConvertible;
					newValue;
					oldValue;
					// const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
					//     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
					// CommandBuffer::MainEditorCommandBuffer().addCommand(ptr);
				}
				else
				{
					const MemberType* unwrapped = &unwrapPointer(member(aReflectedObject));
					componentType* unwrappedComponent = &(aReflectedObject);
					const MemberType  old = oldValue;
					const MemberType  neww = newValue;

					const auto ptr = std::make_shared<VarChanged<componentType, MemberType>>(unwrappedComponent, unwrapped,
						old, neww, arg);
					CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
				}
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
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
						// CommandBuffer::MainEditorCommandBuffer().addCommand(ptr);
					}
					else if constexpr (std::is_pointer_v<declType>)
					{
						newValue;
						oldValue;
						// const auto ptr = std::make_shared<VarChanged<T0, MemberType>>(
						//     aReflectedObject, member(aReflectedObject).get(), oldValue, newValue, arg);
						// CommandBuffer::MainEditorCommandBuffer().addCommand(ptr);
					}
					else
					{
						const auto ptr = std::make_shared<PointerVarChanged<MemberType>>(
							&unwrapPointer(member(aReflectedObject)), oldValue, newValue, arg);
						CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
					}
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
					}
				}
			}
		}

		return changed;
	}

#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code

	template <typename T0>
	bool Reflect(T0& aReflectedObject)
	{
		bool ret{};
		if constexpr (!isReflectableClass<T0>())
		{
			return false;
		}

		auto imp = [&]<typename T1 = float>(const T1 member)
		{
			using MemberType = std::remove_reference_t<decltype(unwrapPointer(member(aReflectedObject)))>;
			using declType = decltype(member(aReflectedObject));

			if constexpr (Reflection::InspectorSyntaxPtr<declType>)
			{
				ret |= member(aReflectedObject)->InspectorView();
				return;
			}
			if constexpr (Reflection::InspectorSyntax<declType>)
			{
				ret |= member(aReflectedObject).InspectorView();
				return;
			}

			const std::string arg = std::string(get_display_name(member)) + ": ";

			ImGui::PushID(arg.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0, 200);
			ImGui::Text(arg.data());
			ImGui::NextColumn();

			ret |= UpdateValue(member, aReflectedObject, arg);
			ImGui::Columns(1);
			ImGui::PopID();
		};
		refl::util::for_each(refl::reflect(aReflectedObject).members, imp);
		ImGui::Separator();
		return ret;
	}

	//Todo document the shit out of this, i have no clue what i have written when looking back
	//More static asserts
	template <typename T0, typename C>
	bool  ReflectSingleValue(T0& member, C& aReflectedObject, const std::string& arg)
	{
		ImGui::PushID(arg.c_str());
		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, 200);
		ImGui::Text(arg.data());
		ImGui::NextColumn();

		using MemberType = std::remove_reference_t<decltype(unwrapPointer(member))>;
		using componentType = std::remove_reference_t<decltype(unwrapPointer(aReflectedObject))>;
		using declType = decltype(member);

		using ptrType = std::add_pointer_t<MemberType>;
		using cmpType = std::add_pointer_t<T0>;

		const auto oldValue = member;
		const bool changed = Reflection::ImGuiReflect(member, arg);

		constexpr bool isConvertible = std::is_convertible_v<cmpType, Component*>;
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
					const MemberType* unwrapped = &unwrapPointer(member);
					componentType* unwrappedComponent = &(aReflectedObject);
					const MemberType  old = oldValue;
					const MemberType  neww = newValue;

					const auto ptr = std::make_shared<VarChanged<componentType, MemberType>>(unwrappedComponent, unwrapped,
						old, neww, arg);
					CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
				}
				if (ImGui::IsItemDeactivatedAfterEdit())
				{
					CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
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
						CommandBuffer::mainEditorCommandBuffer().addCommand(ptr);
					}
					if (ImGui::IsItemDeactivatedAfterEdit())
					{
						CommandBuffer::mainEditorCommandBuffer().getLastCommand()->setMergeBlocker(true);
					}
				}
			}
		}
		ImGui::Columns(1);
		ImGui::PopID();

		return changed;
	};

} // namespace Reflection

#pragma warning(pop)
