struct  DirectionalLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
}; 

struct PointLight
{
    float3 Color;
    float Intensity;
    
    float3 Position;
    float Range;
};

struct SpotLight
{
    float3 Color;
    float Intensity;
    
    float3 Position;
    float3 Direction;
    float Range;
    float InnerConeAngle;
    float OuterConeAngle;
};

cbuffer LightBuffer : register(b3)
{
    DirectionalLight myDirectionalLight;
    PointLight myPointLight[8];
    SpotLight mySpotLight[8];
}