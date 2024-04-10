 
 #include "../Headers/PBRFunctions.hlsli"


DefaultPixelOutput main(BRDF_VS_to_PS input)
{
    
    DefaultPixelOutput result;
    float2 uv = input.UV;
    
    const float4 albedo = colorPass.Sample(defaultSampler, uv);
    const float4 Material = normalPass.Sample(defaultSampler, uv);
    const float4 Normal = materialPass.Sample(defaultSampler, uv);
    const float4 Effect = effectPass.Sample(defaultSampler, uv);
    const float4 vertexNormal = vertexNormalPass.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(normalize(worldPositionPass.Sample(defaultSampler, uv).xyz), 1);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r;
    const float depth = DepthPass.Sample(defaultSampler, uv).r;
    const float4 ssao = SSAOPass.Sample(defaultSampler, uv);
    
    
    
    switch(g_FrameBuffer.FB_RenderMode)
    {
        default:
        case 0:
    {
                result.Color.rgb = 1;
                result.Color.a = 0.0f;
                break;
            }
    
        case 1:
    {
                result.Color.rgb = worldPosition.xyz;
                result.Color.a = 1.0f;
                break;
            }
    
        case 2:
    {
                result.Color.rgb = vertexNormal.rgb;
                result.Color.a = 1.0f;
                break;
            }
    
        case 3:
    {
                result.Color.rgb = albedo.rgb;
                result.Color.a = 1.0f;
                break;
            }
    
        case 4:
    {
                result.Color.rgb = Normal.rgb;
                result.Color.a = 1.0f;
                break;
            }
    
        case 5:
    {
                result.Color.rgb = 1;
                result.Color.a = 1.0f;
                break;
            }
    
        case 6:
    {
                result.Color.rgb = occlusion;
                result.Color.a = 1.0f;
                break;
            }
    
        case 7:
    {
                result.Color.rgb = roughness;
                result.Color.a = 1.0f;
                break;
            }
    
    
        case 8:
    {
                result.Color.rgb = metallic;
                result.Color.a = 1.0f;
                break;
            }
    
        case 9:
    {
                result.Color.rgb = Effect.r;
                result.Color.a = 1.0f;
                break;
            }
        case 10:
    {
                result.Color.rgb = depth;
                result.Color.a = 1.0f;
                break;
            }
        case 11:
    {
                result.Color.rgb = ssao.rgb;
                result.Color.a = 1.0f;
                break;
            }
        case 12:
    {
                result.Color.rgb = GetViewPosition(input.UV).z;
               // result.Color.rgb = (GetViewPosition(input.UV).rgb + 1.0f) / 2.0f;;
                result.Color.a = 1.0f;
                break;
            }
        case 13:
            {
                result.Color.rgb = (GetViewNormal(input.UV).rgb + 1.0f) / 2.0f;
                result.Color.a = 1.0f;
                break;
            }

        case 14:
            {
                result.Color.rgb = normalize(input.position.xyz);
                result.Color.a = 1.0f;
                break;
            }
    
    }
    return result;
}