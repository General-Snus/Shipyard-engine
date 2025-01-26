cbuffer pixelBuffer : register(b0)
{
    float4x4 ColorTransform;
    float4 ColorOffset;
    float4 Grid;
    float2 GridWidth;
    float PremultiplyAlpha;
    float DisableFinalAlpha;
    float3 BackgroundColor;
    bool ForceNearestSampling;
    float2 TextureSize;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv : TEXCOORD0;
};

sampler sampler0;
Texture2D texture0;

float4 main(PS_INPUT input) : SV_Target
{
    float2 uv;
    float2 texel = input.uv * TextureSize;
    if (ForceNearestSampling) 
        uv = (floor(texel) + float2(0.5, 0.5)) / TextureSize;
    else
        uv = input.uv;

    float2 texelEdge = step(texel - floor(texel), GridWidth);
    float isGrid = max(texelEdge.x, texelEdge.y);
    float4 ct = mul(ColorTransform, texture0.Sample(sampler0, uv)) + ColorOffset;
    ct.rgb = ct.rgb * lerp(1.0, ct.a, PremultiplyAlpha);
    ct.rgb += BackgroundColor * (1.0 - ct.a);
    ct.a = lerp(ct.a, 1.0, DisableFinalAlpha);
    ct = lerp(ct, float4(Grid.rgb, 1), Grid.a * isGrid);
    return ct;
}