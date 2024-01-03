#pragma once

// TODO: Fix so this stores colors in 0-1 range as usual.

struct GraphColor
{
	float R = 0;
	float G = 0;
	float B = 0;
	float A = 0;

	const static GraphColor White;
	const static GraphColor Black;

	GraphColor() = default;
	GraphColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {  }

	GraphColor AsBase255() const { return { R, G, B, A }; }
	GraphColor AsNormalized() const { return { R / 255, G / 255, B / 255, A / 255 }; }

	uint32_t AsU32() const { return (static_cast<uint32_t>(A) << 24 | static_cast<uint32_t>(B) << 16 | static_cast<uint32_t>(G) << 8 | static_cast<uint32_t>(R) << 0); }
};