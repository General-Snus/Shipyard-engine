#pragma once
#include <type_traits>
#include <utility>

#define ENABLE_ENUM_BITWISE_OPERATORS(_ENUM_TYPE)                                                                      \
    inline _ENUM_TYPE operator|(const _ENUM_TYPE &a, const _ENUM_TYPE &b)                                              \
    {                                                                                                                  \
        return static_cast<_ENUM_TYPE>(static_cast<std::underlying_type<_ENUM_TYPE>::type>(a) |                        \
                                       static_cast<std::underlying_type<_ENUM_TYPE>::type>(b));                        \
    }                                                                                                                  \
    inline _ENUM_TYPE operator&(const _ENUM_TYPE &a, const _ENUM_TYPE &b)                                              \
    {                                                                                                                  \
        return static_cast<_ENUM_TYPE>(static_cast<std::underlying_type<_ENUM_TYPE>::type>(a) &                        \
                                       static_cast<std::underlying_type<_ENUM_TYPE>::type>(b));                        \
    }                                                                                                                  \
    inline _ENUM_TYPE operator^(const _ENUM_TYPE &a, const _ENUM_TYPE &b)                                              \
    {                                                                                                                  \
        return static_cast<_ENUM_TYPE>(static_cast<std::underlying_type<_ENUM_TYPE>::type>(a) ^                        \
                                       static_cast<std::underlying_type<_ENUM_TYPE>::type>(b));                        \
    }                                                                                                                  \
    inline _ENUM_TYPE operator~(const _ENUM_TYPE &a)                                                                   \
    {                                                                                                                  \
        return static_cast<_ENUM_TYPE>(~static_cast<std::underlying_type<_ENUM_TYPE>::type>(a));                       \
    }                                                                                                                  \
    inline _ENUM_TYPE &operator|=(_ENUM_TYPE &a, const _ENUM_TYPE &b)                                                  \
    {                                                                                                                  \
        a = a | b;                                                                                                     \
        return a;                                                                                                      \
    }                                                                                                                  \
    inline _ENUM_TYPE &operator&=(_ENUM_TYPE &a, const _ENUM_TYPE &b)                                                  \
    {                                                                                                                  \
        a = a & b;                                                                                                     \
        return a;                                                                                                      \
    }                                                                                                                  \
    inline _ENUM_TYPE &operator^=(_ENUM_TYPE &a, const _ENUM_TYPE &b)                                                  \
    {                                                                                                                  \
        a = a ^ b;                                                                                                     \
        return a;                                                                                                      \
    }                                                                                                                  \
    inline bool operator!(const _ENUM_TYPE & val) {                                                                    \
        return static_cast<std::underlying_type<_ENUM_TYPE>::type>(val) != 0;                                          \
    }                                                                                                                  \
    inline bool operator==(_ENUM_TYPE lhs, _ENUM_TYPE rhs) {                                                           \
        return static_cast<std::underlying_type_t<_ENUM_TYPE>>(lhs) ==                                                 \
            static_cast<std::underlying_type_t<_ENUM_TYPE>>(rhs);                                                      \
    }                                                                                                                  \
    inline bool operator!=(_ENUM_TYPE lhs, _ENUM_TYPE rhs) {                                                           \
        return !(lhs == rhs);                                                                                          \
    }                                                                                                                  \
    inline bool is_set(_ENUM_TYPE flags, _ENUM_TYPE flag) {                                                            \
        return bool(flags & flag);                                                                                     \
    }                                                                                                                  \
    inline bool to_bool(_ENUM_TYPE val) {                                                                              \
        return static_cast<std::underlying_type<_ENUM_TYPE>::type>(val) != 0;                                          \
    }                                                                                                                  \
    inline std::underlying_type<_ENUM_TYPE>::type to_underlying(_ENUM_TYPE val) {                                      \
        return static_cast<std::underlying_type<_ENUM_TYPE>::type>(val);                                               \
    }                                                                                                                  \
    inline bool operator&&(_ENUM_TYPE lhs, _ENUM_TYPE rhs) {                                                           \
        return to_bool(lhs & rhs);                                                                           \
    }                                                                                                                  \
    inline bool operator||(_ENUM_TYPE lhs, _ENUM_TYPE rhs) {                                                           \
        return to_bool(lhs | rhs);                                                                           \
    }

#pragma region SmartPointerChecks

template <typename T, class Deleter = std::default_delete<T>>
inline constexpr bool is_smart_pointer = false;
template <typename T, typename Deleter>
inline constexpr bool is_smart_pointer<std::unique_ptr<T, Deleter>> = true;
template <typename T, typename Deleter>
inline constexpr bool is_smart_pointer<const std::unique_ptr<T, Deleter>> = true;
template <typename T>
inline constexpr bool is_smart_pointer<std::unique_ptr<T>> = true;
template <typename T>
inline constexpr bool is_smart_pointer<const std::unique_ptr<T>> = true;
template <typename T>
inline constexpr bool is_smart_pointer<std::shared_ptr<T>> = true;
template <typename T>
inline constexpr bool is_smart_pointer<const std::shared_ptr<T>> = true;

template <class T>
concept SmartPointerType = is_smart_pointer<std::remove_reference_t<T>>;
template <class T>
concept NotSmartPointerType = !is_smart_pointer<std::remove_reference_t<T>>;

template <NotSmartPointerType T>
constexpr T unwrapPointer(const T& not_smart_pointer)
{
	return not_smart_pointer;
}

template <NotSmartPointerType T>
constexpr T& unwrapPointer(T&& not_smart_pointer)
{
	return std::forward<T&>(not_smart_pointer);
}

template <NotSmartPointerType T>
constexpr T& unwrapPointerCreateIfAbsent(T&& not_smart_pointer)
{
	return std::forward<T&>(not_smart_pointer);
}

template <SmartPointerType T>
constexpr typename T::element_type& unwrapPointer(T& smart_pointer)
{
	return std::forward<typename T::element_type&>(*smart_pointer.get());
}

template <SmartPointerType T>
constexpr typename T::element_type& unwrapPointer(const T& smart_pointer)
{
	return std::forward<typename T::element_type&>(*smart_pointer.get());
}

#pragma endregion

template <typename T>
inline constexpr bool is_array = false;
template <typename T, std::size_t N>
inline constexpr bool is_array<std::array<T, N>> = true;
template <typename T, std::size_t N>
inline constexpr bool is_array<const std::array<T, N>> = true;

template <typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F f)
{
	(static_cast<void>(f(std::integral_constant<T, S>{})), ...);
}

template <auto n, typename F>
constexpr void for_sequence(F f)
{
	for_sequence(std::make_integer_sequence<decltype(n), n>{}, f);
}

template <typename... Trest>
struct unique_types;

template <typename T1, typename T2, typename... Trest>
struct unique_types<T1, T2, Trest...> : unique_types<T1, T2>, unique_types<T1, Trest...>, unique_types<T2, Trest...>
{
};

template <typename T1, typename T2>
struct unique_types<T1, T2>
{
	static_assert(!std::is_same_v<T1, T2>, "Types must be unique");
};

template <typename Base, typename Derived>
struct is_derived_from : std::bool_constant<std::is_base_of_v<Base, Derived> &&
		std::is_convertible_v<const volatile Derived*, const volatile Base*>>
{
};

template <typename Base, typename Derived>
inline constexpr bool is_derived_from_v = is_derived_from<Base, Derived>::value;


template<typename Base,typename Derived>
concept IsDerived = std::is_base_of<Base,Derived>::value;

template <typename T, typename... Ts>
constexpr bool Contains = (std::is_same<T, Ts>{} || ...);
template <typename Subset, typename Set>
constexpr bool IsSubsetOf = false;
template <typename... Ts, typename... Us>
constexpr bool IsSubsetOf<std::tuple<Ts...>, std::tuple<Us...>> = (Contains<Ts, Us...> && ...);
