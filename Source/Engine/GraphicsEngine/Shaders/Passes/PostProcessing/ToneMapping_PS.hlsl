#include "../../Headers/ShaderStructs.hlsli"
#include "../../Headers/Tonemaps.hlsli"
#include "../../Headers/Common.hlsli"
#include "../../Headers/PBRFunctions.hlsli"

void UnpackData(float4 packedData, out float2 velocity, out uint id, out float depth)
{
    velocity = packedData.xy;
    id = (uint)packedData.zw; 
}
PostProcessPixelOutput main(BRDF_VS_to_PS input)
{
    PostProcessPixelOutput result;
    const float3 color = Target0_Texture.Sample(defaultSampler, input.UV).rgb;
    result.Color.rgb = (Tonemap_UnrealEngine(color));


    const float2 uv = input.UV;
    const float4 albedo = colorPass.Sample(defaultSampler, uv);
    const float4 Normal = normalPass.Sample(defaultSampler, uv);
    const float4 Material = materialPass.Sample(defaultSampler, uv);
    const float4 Effect = effectPass.Sample(defaultSampler, uv);
    const float4 vertexNormal = vertexNormalPass.Sample(defaultSampler, uv);
    const float4 worldPosition = float4(normalize(worldPositionPass.Sample(defaultSampler, uv).xyz), 1);
    const float metallic = Material.b;
    const float roughness = Material.g;
    const float occlusion = Material.r;
      float depth;
      float2 velocity ;
      uint id  ;
      UnpackData(DepthPass.Sample(defaultSampler, uv), velocity, id, depth);
      
    const float4 ssao = SSAOPass.Sample(defaultSampler, uv); 
    
    
    switch(g_FrameBuffer.FB_RenderMode)
    {
        default:
        case 0:
    {
                result.Color.rgb = (Tonemap_UnrealEngine(color));
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
            case 15:
            {
                result.Color.rb = normalize(velocity);
                result.Color.g = 0.0f;
                result.Color.a = 1.0f;
                break;
            }
            case 16:
            { 
		        float word1 = (id) & 0xFF;
		        float word2 = (id >> 8) & 0xFF; 
		        float word3 = (id >> 24) & 0xFF;
                result.Color.rgb  = normalize(float3((float) word1, (float) word2, (float) word3)) ; 
                result.Color.a = 1.0f;
                break;
            }
    
    }   
    result.Color.a   = 1.0f;
    return result;
    //switch(g_GraphicsSettings.GSB_ToneMap)
    //{
    //    default:
    //        output.Color.rgb = LinearToGamma(color.rgb);
    //        break;
    //    case 1:
    //        output.Color.rgb = LinearToGamma(Tonemap_Reinhard2(color.rgb));
    //        break;
    //    
    //    case 2:
    //        output.Color.rgb = LinearToGamma(Tonemap_UnrealEngine(color.rgb));
    //        break;
    //    
    //    case 3:
    //        output.Color.rgb = LinearToGamma(Tonemap_ACES(color.rgb));
    //        break;
    //    
    //    case 4:
    //        output.Color.rgb = LinearToGamma(Tonemap_Lottes(color.rgb));
    //        break;
    //
    //}  
}
