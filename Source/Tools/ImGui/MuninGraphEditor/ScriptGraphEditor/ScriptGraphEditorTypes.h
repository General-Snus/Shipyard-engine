#pragma once
#include "ScriptGraphEditorTypeRegistry.h"
#include "Tools/ImGui/ImGui/imgui.h"
#include <stdint.h>

#pragma comment(linker, "/include:__MuninGraph_AutoRegEditorTypes")

constexpr uint32_t ConstexprHash(uint32_t in)
{
    constexpr uint32_t r[]{0xdf15236c, 0x16d16793, 0x3a697614, 0xe0fe08e4, 0xa3a53275, 0xccc10ff9,
                           0xb92fae55, 0xecf491de, 0x36e86773, 0x0ed24a6a, 0xd7153d80, 0x84adf386,
                           0x17110e76, 0x6d411a6a, 0xcbd41fed, 0x4b1d6b30};
    uint32_t out{in ^ r[in & 0xF]};
    out ^= std::rotl(in, 020) ^ r[(in >> 010) & 0xF];
    out ^= std::rotl(in, 010) ^ r[(in >> 020) & 0xF];
    out ^= std::rotr(in, 010) ^ r[(in >> 030) & 0xF];
    return out;
}

template <size_t N> constexpr uint32_t ConstexprHash(char const (&str)[N])
{
    uint32_t h{};
    for (uint32_t i{}; i < N; ++i)
        h ^= uint32_t(str[i]) << (i % 4 * 8);
    return ConstexprHash(h);
}

template <size_t N>
constexpr uint32_t constexpr_rand_impl(char const (&file)[N], uint32_t line, uint32_t column = 0x8dc97987)
{
    return ConstexprHash(ConstexprHash(__TIME__) ^ ConstexprHash(file) ^ ConstexprHash(line) ^ ConstexprHash(column));
}

#define RANDOM constexpr_rand_impl(__FILE__, __LINE__)

BEGIN_DECLARE_EDITOR_TYPE(float, Float, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(std::string, String, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(bool, Bool, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(size_t, Size_t, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(int, Int, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(Vector3f, Vector3, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(GameObject, GameObject, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(SY::UUID, UUID, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255), true);
bool TypeEditWidget(std::string_view aUniqueName, const TypedDataContainer &aDataContainer) const override;
std::string ToString(const TypedDataContainer &aDataContainer) const override;
END_DECLARE_EDITOR_TYPE();

BEGIN_DECLARE_EDITOR_TYPE(void *, Internal_VoidPtr, GraphColor((float)RANDOM, (float)RANDOM, (float)RANDOM, 255),
                          false);
FORCEINLINE bool IsInternal() const override
{
    return true;
}
END_DECLARE_EDITOR_TYPE();
