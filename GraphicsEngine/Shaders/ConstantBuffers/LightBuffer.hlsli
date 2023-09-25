struct  DirectionalLight
{
    float3 Color;   //12
    float Intensity;//4
    
    float3 Direction; //12
    float Pad;        //4
}; 

struct PointLight
{
    float3 Color; //12
    float Intensity;//4
    
    float3 Position; //12
    float Range;    //4
};

struct SpotLight
{
    float3 Color;   //12
    float Intensity;//4
    
    float3 Position; //12
    float Range;    //4
    
    float3 Direction;       //12
    float InnerConeAngle;   //4
    
    float OuterConeAngle; //4
    float3 Pad; //12
};

cbuffer LightBuffer : register(b3)
{
    DirectionalLight myDirectionalLight;    //32
    PointLight myPointLight[8];             //32 * 8
    SpotLight mySpotLight[8];               //64*8
}