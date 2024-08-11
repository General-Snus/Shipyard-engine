#pragma once 
#include <map> 
#include "LinearAlgebra/Vectors.hpp" 
#include <unordered_map>
#include <Tools/Utilities/Math.hpp>


#undef min
#undef max

class ColorManager
{
public:
	friend class Color;
	friend class ColorPresets;
	~ColorManager();
	static void InitializeDefaultColors();
	static void LoadColorsFromFile(const std::filesystem::path& path);
	static void DumpToFile(const std::filesystem::path& path);

	static Vector4f& CreateColor(const size_t& color, Vector4f colorValue);
	static Vector4f CreateColor(const std::string& identifier, Vector4f colorValue, bool lock = false);

	static Vector4f& GetColor(const size_t& color);
	static Vector4f& GetColor(const std::string& identifier);

	static size_t GetHash(const Vector4f& colorValue)
	{
		return std::hash<Vector4f>()(colorValue);
	}

	typedef std::string NamedIdentifier;
	static inline std::map<NamedIdentifier, Vector4f> m_NamedColor;
private:

	struct BlendStruct
	{
		NamedIdentifier BlendDevice1;
		NamedIdentifier BlendDevice2;
		float Blend;
	};
	static inline std::map<NamedIdentifier, BlendStruct> m_BlendedColor;
	static inline std::map<NamedIdentifier, bool> m_Locks;


	typedef size_t unNamedIdentifier;
	static inline std::unordered_map<unNamedIdentifier, Vector4f> m_UnNamedColor;
};

class Color
{
public:
	std::string m_ColorName;
	size_t m_Context;

	explicit Color();

	explicit Color(const Vector4f& color);
	explicit Color(const Vector3f& color);

	Color(float red, float green, float blue, float alpha = 1.0f);

	Color(const std::string& hexOrName);
	Color(const char* hexOrName);



	Color& operator=(const std::string& hex);
	bool operator==(const Color& other) const;

	Vector4f operator()() const;
	Vector4f GetRGBA() const;

	Vector3f GetRGB() const;
	//Mutable
	float* GetM_RGBA() const;

	Vector4f GetHSV() const;

	std::string GetHex() const;
	void SetColor(Vector4f color);

	static Vector4f RGBtoHSV(const Color& colorIn);
	static Vector4f RGBLerp(const Color& colorIn1, const Color& colorIn2, float blend);

	static Color HSVtoRGB(const Color& colorIn);

	static Color FromHex(const std::string& hex);
};

