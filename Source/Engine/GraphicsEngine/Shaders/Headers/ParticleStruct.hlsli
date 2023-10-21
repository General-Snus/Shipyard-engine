
struct ParticlePixelOutput
{
    float4 Color : SV_TARGET; // carry till geo 
};

struct ParticleGeometryToPixel
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float LifeTime : LIFETIME;
    float2 UV : TEXCOORD;
};

struct ParticleVertexData
{
    float4 Position : POSITION;
    float4 Color : COLOR;
    float3 Velocity : VELOCITY;
    float3 Scale : SCALE;
    float LifeTime : LIFETIME;
};