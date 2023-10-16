#include "../../../Headers/ShaderStructs.hlsli"
  
struct depthOut
{ 
    float4 color : SV_Target;
    float depth : SV_Depth;
};

depthOut main(GSOutput input)
{
    depthOut output;
    float dist = distance(FB_CameraPosition, input.posCS.xyz);
    
    output.depth = dist; 
    return output;  
}


depthOut main(VertexOutput input)
{
    depthOut output;
    float dist = distance(FB_CameraPosition, input.Position.xyz);
    
    output.color = float4(dist,0,0,1);
    output.depth = dist; 
    return output;
}