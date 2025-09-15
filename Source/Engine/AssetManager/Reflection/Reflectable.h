#pragma once
#include <Tools/Reflection/refl.hpp> 
#include <string>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include "ReflectionTemplate.h"

namespace Reflection {

	template <typename T, typename Type = std::remove_cvref_t<T>>
	inline constexpr bool isStdType = get_name(refl::reflect<Type>()).template substr<0, 5>() == "std::";

	class TypeInfo
	{
	public:
		// instances can be obtained only through calls to Get()
		template <typename T>
		static const TypeInfo& Get()
		{
			static TypeInfo ti(refl::reflect<T>());

			static bool added = []()
			{
				//if constexpr (IsComponent<T>)
				//{
				//	ti.isComponent = true;
				//}
				//if constexpr (std::is_base_of_v<AssetBase, T>)
				//{
				//	ti.isAsset = true;
				//}
				allTypes.push_back(ti);
				return true;
			}();

			return ti;
		}

		const std::string& Name() const
		{
			return name_;
		}

		const std::type_info* TypeID() const
		{
			return typeId;
		}
		static const std::vector<TypeInfo>& GetAllTypes()
		{
			return allTypes;
		}

		bool isComponent = false;
		bool isAsset = false;

	private:
		std::string                         name_;
		const std::type_info* typeId;
		static inline std::vector<TypeInfo> allTypes;

		template <typename T, typename... Fields>
		TypeInfo(refl::type_descriptor<T> td) : name_(td.name), typeId(&typeid(T))
		{
		}
	};
	template <class T, typename RawType = std::remove_cvref_t<T>>
	constexpr bool isReflectableClass()
	{
		if constexpr (std::is_class_v<RawType> && refl::is_reflectable<RawType>() && !std::is_fundamental_v<RawType> &&
			!std::is_array_v<RawType>)
		{
			return !isStdType<RawType>;
		}
		return false;
	}

	template <typename T>
	constexpr bool isReflectableClass_v = isReflectableClass<T>();
	template <class T>
	concept ReflectableClass = isReflectableClass<T>();
}

template<class Derived>
class Reflectable {
public:
	bool Reflect()
	{
		return Reflection::Reflect<Derived>(static_cast<Derived&>(*this));
	}

	const Reflection::TypeInfo& GetTypeInfo() const
	{
		using safeType = ::refl::trait::remove_qualifiers_t<Derived>;
		return Reflection::TypeInfo::Get<safeType>();
	}
};

#define reflectable( className )			\
using Reflectable<className>::Reflect;		\
using Reflectable<className>::GetTypeInfo;	 



#define defaultComponentInspector( )																						\
bool InspectorView() override  																					\
{  																												\
	if (!Component::InspectorView())  																			\
	{  																											\
		return false; 																							\
	}  																											\
	Reflect();  											\
	return false;																								\
}

#pragma warning(push)
#pragma warning(disable : 4702)

#pragma warning(pop)
