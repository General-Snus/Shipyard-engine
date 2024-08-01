#include "Color.h" 

#include <fstream> 
#include "LinearAlgebra/Vectors.hpp"
#include <string>
#include <Tools/ThirdParty/nlohmann/json.hpp> 
#include "Engine/AssetManager/AssetManager.h"

#undef min
#undef max

void ColorManager::InitializeDefaultColors()
{
	CreateColor("Black", Vector4f(0, 0, 0, 1), true);
	CreateColor("White", Vector4f(1, 1, 1, 1), true);
	CreateColor("Red", Vector4f(1, 0, 0, 1), true);
	CreateColor("Green", Vector4f(0, 1, 0, 1), true);
	CreateColor("Blue", Vector4f(0, 0, 1, 1), true);
	CreateColor("Yellow", Vector4f(1, 1, 0, 1), true);
	CreateColor("Cyan", Vector4f(0, 1, 1, 1), true);
	CreateColor("Magenta", Vector4f(1, 0, 1, 1), true);
	CreateColor("Gray", Vector4f(0.5f, 0.5f, 0.5f, 1), true);
	CreateColor("Orange", Vector4f(1, 0.5f, 0, 1), true);
	CreateColor("Purple", Vector4f(0.5f, 0, 0.5f, 1), true);
	CreateColor("Brown", Vector4f(0.6f, 0.3f, 0.2f, 1), true);
	CreateColor("Pink", Vector4f(1, 0.75f, 0.8f, 1), true);
	CreateColor("Lime", Vector4f(0.75f, 1, 0, 1), true);
	CreateColor("Olive", Vector4f(0.5f, 0.5f, 0, 1), true);
	CreateColor("Navy", Vector4f(0, 0, 0.5f, 1), true);
	CreateColor("Teal", Vector4f(0, 0.5f, 0.5f, 1), true);
	CreateColor("Maroon", Vector4f(0.5f, 0, 0, 1), true);
	CreateColor("Silver", Vector4f(0.75f, 0.75f, 0.75f, 1), true);
	CreateColor("Gold", Vector4f(1, 0.84f, 0, 1), true);
}

void ColorManager::LoadColorsFromFile(const std::filesystem::path& path)
{
	auto contentPath = AssetManager::AssetPath / path;
	if (!std::filesystem::exists(contentPath))
	{
		Logger::Warn("Color file not found: " + contentPath.string());
		return;
	}

	std::ifstream file(contentPath);
	assert(file.is_open());

	if (!file.is_open())
	{
		Logger::Warn("Color file could not be read: " + contentPath.string());
		return;
	}

	nlohmann::json json = nlohmann::json::parse(file);
	file.close();

	if (json.contains("NamedColors"))
	{
		for (const auto& [name, value] : json["NamedColors"].items())
		{
			CreateColor(name, { value[0], value[1], value[2], value[3] });
		}
	}

}

void ColorManager::DumpToFile(const std::filesystem::path& path)
{
	auto contentPath = AssetManager::AssetPath / path;

	if (!std::filesystem::exists(contentPath))
	{
		std::error_code ec;
		if (!std::filesystem::create_directories(contentPath.root_directory(), ec))
		{
			Logger::Warn("Failure when dumping color data to memory: " + ec.message());
		}
	}
	nlohmann::json j;

	for (const auto& [name, value] : ColorManager::m_NamedColor)
	{
		j["NamedColors"][name] = value;
	}

	std::ofstream o(contentPath);
	o << std::setw(4) << j << std::endl;

}

Vector4f& ColorManager::CreateColor(const size_t& color, Vector4f colorValue)
{
	return m_UnNamedColor[color] = colorValue;
}

Vector4f ColorManager::CreateColor(const std::string& identifier, Vector4f colorValue, bool lock)
{
	if (lock)
	{
		m_Locks[identifier] = true;
	}

	return m_NamedColor[identifier] = colorValue;
}

Vector4f& ColorManager::GetColor(const size_t& color)
{
	if (m_UnNamedColor.contains(color))
	{
		return m_UnNamedColor[color];
	}
	return  m_NamedColor["White"];
}

Vector4f& ColorManager::GetColor(const std::string& identifier)
{
	bool isLocked = m_Locks.contains(identifier);

	isLocked;
	//TODO FIX locked colors from change during runtime
	if (m_NamedColor.contains(identifier))
	{
		return m_NamedColor[identifier];
	}
	return  m_NamedColor["White"];
}

Color::Color() : m_ColorName("White"), m_Context(ColorManager::GetHash({ 1,1,1,1 }))
{
	ColorManager::CreateColor(m_ColorName, { 1,1,1,1 });
}

Color::Color(const Vector4f& color) : m_Context(ColorManager::GetHash(color))
{
	ColorManager::CreateColor(m_Context, color);
}

Color::Color(const Vector3f& color) : m_Context(ColorManager::GetHash({ color.x,color.y,color.z,1.f }))
{
	ColorManager::CreateColor(m_Context, { color.x,color.y,color.z,1.f });
}

Color::Color(float red, float green, float blue, float alpha) : m_Context(ColorManager::GetHash({ red,green,blue,alpha }))
{ 
	ColorManager::CreateColor(m_Context, { red,green,blue,alpha });
}

Color::Color(const std::string& hexOrName)
{
	if (hexOrName.front() != '#')
	{
		if(ColorManager::m_NamedColor.contains(hexOrName))
		{
			m_ColorName = hexOrName;
			m_Context = ColorManager::GetHash(ColorManager::m_NamedColor[hexOrName]);
			return;
		}
		*this = Color(); 
	}
	else
	{
		*this = FromHex(hexOrName);
	}
}

Color::Color(const char* hexOrName) : Color(std::string(hexOrName))
{
} 

Color& Color::operator=(const std::string& hex)
{
	return *this = FromHex(hex);
}

Vector4f Color::operator()() const
{
	return !m_ColorName.empty() ? ColorManager::GetColor(m_ColorName) : ColorManager::GetColor(m_Context);
}

Vector3f Color::GetRGB() const
{
	const auto& ref = !m_ColorName.empty() ? ColorManager::GetColor(m_ColorName) : ColorManager::GetColor(m_Context);
	return { ref.x, ref.y, ref.z };
}

bool Color::operator==(const Color& other) const
{
	if (other.m_ColorName == m_ColorName || m_Context == other.m_Context)
	{
		return true;
	}
	return false;
	/*return IsApproximate(r, other.r) &&
			IsApproximate(g, other.g) &&
			IsApproximate(b, other.b) &&
			IsApproximate(a, other.a);*/
}

Vector4f Color::GetRGBA() const
{
	return !m_ColorName.empty() ? ColorManager::GetColor(m_ColorName) : ColorManager::GetColor(m_Context);
}

float* Color::GetM_RGBA() const
{
	return !m_ColorName.empty() ? &ColorManager::GetColor(m_ColorName).x : &ColorManager::GetColor(m_Context).x;
}

Vector4f Color::GetHSV() const
{
	return RGBtoHSV(*this);
}

std::string Color::GetHex() const
{
	const auto color = GetRGBA();

	auto ri = static_cast<int>(color.x * 255.0f);
	auto gi = static_cast<int>(color.y * 255.0f);
	auto bi = static_cast<int>(color.z * 255.0f);
	auto ai = static_cast<int>(color.w * 255.0f);
	char hex[10];
	snprintf(hex, sizeof(hex), "#%02x%02x%02x%02x", ri, gi, bi, ai);
	return std::string(hex);
}

void Color::SetColor(Vector4f color)
{
	m_ColorName.clear();
	m_Context = ColorManager::GetHash(color);
	ColorManager::CreateColor(m_Context, color);
}

Vector4f Color::RGBtoHSV(const Color& colorIn)
{
	const auto color = colorIn.GetRGBA();
	float rgbMin = std::min(std::min(color.x, color.y), color.z);
	float rgbMax = std::max(std::max(color.x, color.y), color.z);

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
		if (rgbMax == color.x) {
			h = (color.y - color.z) / d + (color.y < color.z ? 6 : 0);
		}
		else if (rgbMax == color.y) {
			h = (color.z - color.x) / d + 2;
		}
		else if (rgbMax == color.z) {
			h = (color.x - color.y) / d + 4;
		}
		h /= 6;
	}
	return Vector4(h, s, v, color.w);
}

Vector4f Color::RGBLerp(const Color& colorIn1, const Color& colorIn2, float blend)
{
	Vector4f a = colorIn1.GetRGBA();
	Vector4f b = colorIn2.GetRGBA();
	Vector4f c{};

	blend = std::clamp(blend, 0.f, 1.f);

	c.x = a.x + (b.x - a.x) * blend;
	c.y = a.y + (b.y - a.y) * blend;
	c.z = a.z + (b.z - a.z) * blend;
	c.w = a.w + (b.w - a.w) * blend;

	return c;
}

Color Color::HSVtoRGB(const Color& colorIn)
{
	const auto color = colorIn.GetRGBA();

	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;

	const int i = static_cast<int>(color.x * 6);
	const float f = color.x * 6 - i;
	const float p = color.z * (1 - color.y);
	const float q = color.z * (1 - f * color.y);
	const float t = color.z * (1 - (1 - f) * color.y);

	switch (i % 6) {
	case 0: r = color.z, g = t, b = p; break;
	case 1: r = q, g = color.z, b = p; break;
	case 2: r = p, g = color.z, b = t; break;
	case 3: r = p, g = q, b = color.z; break;
	case 4: r = t, g = p, b = color.z; break;
	case 5: r = color.z, g = p, b = q; break;
	}

	return { r, g, b, color.w };
}

Color Color::FromHex(const std::string& hex)
{

	if (hex.front() != '#' || (hex.length() != 7 && hex.length() != 9))
	{
		throw std::invalid_argument("Invalid hex color format");
	}

	float r = static_cast<float>(std::stoi(hex.substr(1, 2), nullptr, 16)) / 255.0f;
	float g = static_cast<float>(std::stoi(hex.substr(3, 2), nullptr, 16)) / 255.0f;
	float b = static_cast<float>(std::stoi(hex.substr(5, 2), nullptr, 16)) / 255.0f;
	float a = hex.length() == 9 ? static_cast<float>(std::stoi(hex.substr(7, 2), nullptr, 16)) / 255.0f : 1.0f;

	return Color(r, g, b, a);
}