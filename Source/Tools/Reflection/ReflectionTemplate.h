#pragma once
#include "Tools/ImGui/ImGui/imgui.h"
#include "Tools/Reflection/refl.hpp"

class AssetBase;

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
};

// define a convenience macro to autoimplement GetTypeInfo()
#define MYLIB_REFLECTABLE() \
    virtual const TypeInfo& GetTypeInfo() const override \
    { \
        return TypeInfo::Get<::refl::trait::remove_qualifiers_t<decltype(*this)>>(); \
    }
