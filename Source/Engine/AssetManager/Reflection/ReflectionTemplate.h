#pragma once
#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/ImGui/ImGui/imgui_internal.h"
#include "Tools/Reflection/refl.hpp"
#include "Tools/Utilities/LinearAlgebra/Vectors.hpp"

class AssetBase;



namespace Reflection
{

	template <typename T>
	void ImGuiReflect(T& ref,const std::string& identifier)
	{
		identifier;
		ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
	}

	template <typename T>
	void ImGuiReflect(const T& ref,const std::string& identifier)
	{
		identifier;
		ImGui::TextWrapped(refl::runtime::debug_str(ref).c_str());
	}

	inline void ImGuiReflect(bool& ref,const std::string& identifier)
	{
		ImGui::Checkbox(std::format("##{}",identifier).c_str(),&ref);
	}

	inline void ImGuiReflect(Vector2<float>& ref,const std::string& identifier)
	{
		ImGui::DragFloat2(std::format("##{}",identifier).c_str(),&ref.x);
	}

	inline void ImGuiReflect(Vector3<float>& ref,const std::string& identifier)
	{
		ImGui::DragFloat3(std::format("##{}",identifier).c_str(),&ref.x);
	}

	inline void ImGuiReflect(Vector4<float>& ref,const std::string& identifier)
	{
		ImGui::DragFloat4(std::format("##{}",identifier).c_str(),&ref.x);
	}

	inline void ImGuiReflect(float& ref,const std::string& identifier)
	{
		ImGui::DragFloat(std::format("##{}",identifier).c_str(),&ref);
	}













































	template <typename T> struct is_shared_ptr : std::false_type {};
	template <typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
	template <typename T> concept IsSharedPtr = is_shared_ptr<T>::value;

	template<typename T>
	concept pointerSyntax = requires(T a) {
		a->GetTypeInfo();
	};

	template<typename T>
	concept InspectorSyntax = requires(T a) {
		a.InspectorView();
	};
	template<typename T>
	concept InspectorSyntaxPtr = requires(T a) {
		a->InspectorView();
	};
}
struct serializable : refl::attr::usage::field,refl::attr::usage::function
{
};
template <typename T>
void serialize(std::ostream& os,T&& value)
{
	// iterate over the members of T
	for_each(refl::reflect(value).members,[&](auto member)
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

class TypeInfo
{
public:
	// instances can be obtained only through calls to Get()
	template <typename T>
	static const TypeInfo& Get()
	{
		// here we create the singleton instance for this particular type
		static const TypeInfo ti(refl::reflect<T>());
		return ti;
	}

	const std::string& Name() const
	{
		return name_;
	}

private:

	// were only storing the name for demonstration purposes,
	// but this can be extended to hold other properties as well
	std::string name_;

	// given a type_descriptor, we construct a TypeInfo
	// with all the metadata we care about (currently only name)
	template <typename T,typename... Fields>
	TypeInfo(refl::type_descriptor<T> td)
		: name_(td.name)
	{
	}
};

class Reflectable
{
public:
	virtual const TypeInfo& GetTypeInfo() const = 0;
	virtual void InspectorView();
	template<typename TypeToReflect>
	void Reflect(this auto& aReflectedObject);
	template <typename TypeToReflect>
	void Reflect(TypeToReflect& aReflectedObject);
};



inline void Reflectable::InspectorView()
{
}

// define a convenience macro to autoimplement GetTypeInfo()
#define MYLIB_REFLECTABLE() \
	virtual const TypeInfo& GetTypeInfo() const override \
	{ \
		return TypeInfo::Get<::refl::trait::remove_qualifiers_t<decltype(*this)>>(); \
	} 

template <typename TypeToReflect>
inline void Reflectable::Reflect(this auto& aReflectedObject)
{
	auto TypeReflector = [&] <typename T>(this auto & self)
	{
		auto imp = [&]<typename T0>(T0 member)
		{
			const std::string arg = std::string(get_display_name(member)) + ": ";
			ImGui::PushID(arg.c_str());
			ImGui::Columns(2);
			ImGui::SetColumnWidth(0,200);
			ImGui::Text(arg.data());
			ImGui::NextColumn(); 
			
			Reflection::ImGuiReflect(member(aReflectedObject),arg);
			ImGui::Columns(1);
			ImGui::PopID();

			ImGui::Indent(16.f);
			if constexpr (Reflection::InspectorSyntaxPtr<decltype(member(aReflectedObject))>)
			{
				member(aReflectedObject)->InspectorView();
			}
			if constexpr (Reflection::InspectorSyntax<decltype(member(aReflectedObject))>)
			{
				member(aReflectedObject).InspectorView();
			}

			ImGui::Unindent(16.0f);

		};
		refl::util::for_each(refl::reflect<T>().members,imp);
	};
	TypeReflector.template operator() < TypeToReflect > ();
	ImGui::Separator();
}

template <typename TypeToReflect>
inline void Reflectable::Reflect(TypeToReflect& aReflectedObject)
{
	auto TypeReflector = [&] <typename T>(this auto & self)
	{
		auto imp = [&]<typename T0>(T0 member)
		{
			const std::string arg = std::string(get_display_name(member)) + ": ";
			ImGui::TextWrapped(arg.data());
			ImGui::SameLine();
			Reflection::ImGuiReflect(member(aReflectedObject),arg);

			ImGui::Indent(16.f);
			if constexpr (Reflection::InspectorSyntaxPtr<decltype(member(aReflectedObject))>)
			{
				member(aReflectedObject)->InspectorView();
			}
			if constexpr (Reflection::InspectorSyntax<decltype(member(aReflectedObject))>)
			{
				member(aReflectedObject).InspectorView();
			}

			ImGui::Unindent(16.0f);

		};
		refl::util::for_each(refl::reflect<T>().members,imp);
	};
	TypeReflector.template operator() < TypeToReflect > ();
	ImGui::Separator();
}

