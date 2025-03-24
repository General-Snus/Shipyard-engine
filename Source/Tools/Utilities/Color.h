#pragma once
#include "LinearAlgebra/Vectors.hpp"
#include <Tools/Utilities/Math.hpp>
#include <map>
#include <unordered_map>

#undef min
#undef max

#include <Tools/Utilities/System/ServiceLocator.h>
#include <Tools/Utilities/System/SingletonTemplate.h>
#define ColorManagerInstance ServiceLocator::Instance().GetService<ColorManager>()

class ColorManager : public Singleton
{
  public:
    friend class Color;
    friend class ColorPresets;
    void InitializeDefaultColors();
    void LoadColorsFromFile(const std::filesystem::path &path);
    void DumpToFile(const std::filesystem::path &path);

    Vector4f &CreateColor(const size_t &color, Vector4f colorValue);
    Vector4f CreateColor(const std::string &identifier, Vector4f colorValue, bool lock = false);

    bool GetColor(const size_t &color, Vector4f &outColor);

    bool GetColor(const std::string &identifier, Vector4f &outColor);

    Vector4f &GetColor(const size_t &color);
    Vector4f &GetColor(const std::string &identifier);

    static size_t GetHash(const Vector4f &colorValue)
    {
        return std::hash<Vector4f>()(colorValue);
    }

    typedef std::string NamedIdentifier;
    std::map<NamedIdentifier, Vector4f> m_NamedColor;

  private:
    struct BlendStruct
    {
        NamedIdentifier BlendDevice1;
        NamedIdentifier BlendDevice2;
        float Blend;
    };
    std::map<NamedIdentifier, BlendStruct> m_BlendedColor;
    std::map<NamedIdentifier, bool> m_Locks;

    typedef size_t unNamedIdentifier;
    std::unordered_map<unNamedIdentifier, Vector4f> m_UnNamedColor;
};

class Color
{
  public:
    std::string m_ColorName;
    size_t m_Context;

    explicit Color();

    explicit Color(const Vector4f &color);
    explicit Color(const Vector3f &color);

    Color(float red, float green, float blue, float alpha = 1.0f);

    Color(const std::string &hexOrName);
    Color(const char *hexOrName);

    Color &operator=(const std::string &hex);
    bool operator==(const Color &other) const;

    Vector4f operator()() const;
    Vector4f GetRGBA() const;

    Vector3f GetRGB() const;
    // Mutable
    float *GetM_RGBA() const;

    Vector4f GetHSV() const;

    std::string GetHex() const;
    void SetColor(Vector4f color);

    static Vector4f RGBtoHSV(const Color &colorIn);
    static Vector4f RGBLerp(const Color &colorIn1, const Color &colorIn2, float blend);

    static Color HSVtoRGB(const Color &colorIn);

    static Color FromHex(const std::string &hex);
};
