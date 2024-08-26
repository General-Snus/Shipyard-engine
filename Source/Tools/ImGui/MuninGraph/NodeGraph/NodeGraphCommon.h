#pragma once
#include "UID.h"
#include <cstdint>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

struct ImDrawList;

struct GraphColor
{
    float R = 0;
    float G = 0;
    float B = 0;
    float A = 0;

    const static GraphColor White;
    const static GraphColor Black;

    GraphColor() = default;
    GraphColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a)
    {
    }

    GraphColor AsBase255() const
    {
        return {R, G, B, A};
    }
    GraphColor AsNormalized() const
    {
        return {R / 255, G / 255, B / 255, A / 255};
    }

    uint32_t AsU32() const
    {
        return (static_cast<uint32_t>(A) << 24 | static_cast<uint32_t>(B) << 16 | static_cast<uint32_t>(G) << 8 |
                static_cast<uint32_t>(R) << 0);
    }
};

enum class PinDirection
{
    // Any, - ImNodeEd doesn't support Any direction.
    Input,
    Output
};

enum GraphNodeFlags_
{
    GraphNodeFlag_None = 0,
    // This node is Internal Only and will not be shown automatically.
    GraphNodeFlag_Internal = 1 << 1,
    // This node is Unique and can only exist once per graph;
    GraphNodeFlag_Unique = 1 << 2,
    // This node can have Dynamically added Input Pins.
    GraphNodeFlag_DynamicInputPins = 1 << 3,
    // This node can have Dynamically added Output Pins.
    GraphNodeFlag_DynamicOutputPins = 1 << 4,
    // This node can allows Dynamic pins to be added and removed in the editor.
    GraphNodeFlag_EditableInputPins = 1 << 5,
    // This node can allows Dynamic pins to be added and removed in the editor.
    GraphNodeFlag_EditableOutputPins = 1 << 6,
};

typedef int GraphNodeFlags;
