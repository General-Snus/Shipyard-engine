#pragma once 
#include "LinearAlgebra/Vectors.hpp"
#include <string>

#undef min
#undef max

class Color {
public: 
	float r{};
	float g{};
	float b{};
	float a = 1;


	Color() : r(0.0f),g(0.0f),b(0.0f),a(1.0f) {}

	explicit Color(const Vector4f color) : r(color.x),g(color.y),b(color.z),a(color.w) {}
	explicit Color(const Vector3f color) : r(color.x),g(color.y),b(color.z),a(1.f) {}

	Color(float red,float green,float blue,float alpha = 1.0f)
		: r(red),g(green),b(blue),a(alpha) {}

	Color(const std::string& hex) { *this = FromHex(hex); }

	void operator=(const std::string& hex) {
		*this = FromHex(hex); 
	}

	bool operator==(const Color& other) const {
		return std::abs(r - other.r) < 0.001f &&
			std::abs(g - other.g) < 0.001f &&
			std::abs(b - other.b) < 0.001f &&
			std::abs(a - other.a) < 0.001f;
	}
	 
	Vector4f operator()() const { return Vector4f(r,g,b,a); } 
	Vector4f GetRGBA() const { return Vector4f(r,g,b,a); }
	Vector3f GetRGB() const { return Vector3f(r,g,b); }

	Vector4f GetHSV() const {
		Color hsv = RGBtoHSV(*this);
		return Vector4f(hsv.r,hsv.g,hsv.b,a);
	}
	 
	std::string GetHex() const {
		auto ri = static_cast<int>(r * 255.0f);
		auto gi = static_cast<int>(g * 255.0f);
		auto bi = static_cast<int>(b * 255.0f);
		auto ai = static_cast<int>(a * 255.0f);
		char hex[10];
		snprintf(hex,sizeof(hex),"#%02x%02x%02x%02x",ri,gi,bi,ai);
		return std::string(hex);
	}

	static Color RGBtoHSV(const Color& color) 
	{ 
		float rgbMin = std::min(std::min(color.r,color.g),color.b);
		float rgbMax = std::max(std::max(color.r,color.g),color.b);

		float h = 0.0f;
		float s;
		float v;
		v = rgbMax;

		float d = rgbMax - rgbMin;
		s = rgbMax == 0 ? 0 : d / rgbMax;

		if (rgbMax == rgbMin) {
			h = 0; // achromatic
		}
		else {
			if (rgbMax == color.r) {
				h = (color.g - color.b) / d + (color.g < color.b ? 6 : 0);
			}
			else if (rgbMax == color.g) {
				h = (color.b - color.r) / d + 2;
			}
			else if (rgbMax == color.b) {
				h = (color.r - color.g) / d + 4;
			}
			h /= 6;
		}
		return Color(h,s,v,color.a);
	}
	 
	static Color HSVtoRGB(const Color& color) {
		float r;
		float g;
		float b;

		int i = int(color.r * 6);
		float f = color.r * 6 - i;
		float p = color.b * (1 - color.g);
		float q = color.b * (1 - f * color.g);
		float t = color.b * (1 - (1 - f) * color.g);

		switch (i % 6) {
		case 0: r = color.b,g = t,b = p; break;
		case 1: r = q,g = color.b,b = p; break;
		case 2: r = p,g = color.b,b = t; break;
		case 3: r = p,g = q,b = color.b; break;
		case 4: r = t,g = p,b = color.b; break;
		case 5: r = color.b,g = p,b = q; break;
		}

		return Color(r,g,b,color.a);
	}

	static Color FromHex(const std::string& hex) {

		if (hex.front() != '#' || (hex.length() != 7 && hex.length() != 9)) 
		{
			throw std::invalid_argument("Invalid hex color format");
		}

		float r = static_cast<float>(std::stoi(hex.substr(1,2),nullptr,16)) / 255.0f;
		float g = static_cast<float>(std::stoi(hex.substr(3,2),nullptr,16)) / 255.0f;
		float b = static_cast<float>(std::stoi(hex.substr(5,2),nullptr,16)) / 255.0f;
		float a = hex.length() == 9 ? static_cast<float>(std::stoi(hex.substr(7,2),nullptr,16)) / 255.0f : 1.0f;

		return Color(r,g,b,a);
	} 
};