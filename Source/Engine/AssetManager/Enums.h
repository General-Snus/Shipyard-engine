#pragma once

#include <Tools/Reflection/magic_enum/magic_enum.hpp>
#include <Tools/Reflection/magic_enum/magic_enum_flags.hpp>
enum class eTextureType
{
	ColorMap = 0,
	NormalMap = 1,
	MaterialMap = 2,
	EffectMap = 3,
	ParticleMap = 4,
	CubeMap = 100
};

enum class   Layer
{
	None = 1 << 0,
	Default = 1<< 1,
	Enemy = 1 << 2,
	Player = 1 << 3,
	Entities = Enemy | Player,
	Projectile = 1 << 4,
	Environment = 1 << 5,
	AllLayers = INT32_MAX
};
template <>
struct magic_enum::customize::enum_range<Layer> {
	static constexpr bool is_flags = true;
};

template<class T> inline Layer operator~ (Layer a) { return (Layer)~(int)a; }
template<class T> inline Layer operator| (Layer a,Layer b) { return (Layer)((int)a | (int)b); }
template<class T> inline Layer operator& (Layer a,Layer b) { return (Layer)((int)a & (int)b); }
template<class T> inline Layer operator^ (Layer a,Layer b) { return (Layer)((int)a ^ (int)b); }
template<class T> inline Layer& operator|= (Layer a,Layer b) { return (Layer&)((int&)a |= (int)b); }
template<class T> inline Layer& operator&= (Layer a,Layer b) { return (Layer&)((int&)a &= (int)b); }
template<class T> inline Layer& operator^= (Layer a,Layer b) { return (Layer&)((int&)a ^= (int)b); }

