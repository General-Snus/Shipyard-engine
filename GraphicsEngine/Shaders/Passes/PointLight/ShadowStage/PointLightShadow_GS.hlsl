#include "../../../Headers/ShaderStructs.hlsli"



[maxvertexcount(18)]
void main(triangle VertexOutput input[3], inout TriangleStream<GSOutput> output)
{
    const float4x4 viewMatrixInverse[6] =
    {
        //Right
        float4x4(
            float4(0, 0, -1, 0),
            float4(0, 1, 0, 0),
            float4(1, 0, 0, 0),
            float4(0, 0, 0, 1)),
        
        //left
        float4x4(
            float4(0, 0, 1, 0),
            float4(0, 1, 0, 0),
            float4(-1, 0, 0, 0),
            float4(0, 0, 0, 1)),
        
        //up
        float4x4(
            float4(1, 0, 0, 0),
            float4(0, 0, -1, 0),
            float4(0, 1, 0, 0),
            float4(0, 0, 0, 1)),
        
        //down
        float4x4(
            float4(1, 0, 0, 0),
            float4(0, 0, 1, 0),
            float4(0, -1, 0, 0),
            float4(0, 0, 0, 1)),
        
        //Forward
        float4x4(
            float4(1, 0, 0, 0),
            float4(0, 1, 0, 0),
            float4(0, 0, 1, 0),
            float4(0, 0, 0, 1)),
                
        //back
        float4x4(
            float4(-1, 0, 0, 0),
            float4(0, 1, 0, 0),
            float4(0, 0, -1, 0),
            float4(0, 0, 0, 1)),
    };
    
    [unroll]
    for (uint count = 0; count < 6; count++)
    {
        [unroll]
        for (uint i = 0; i < 3; i++)
        {
            GSOutput element;
            element.posCS = mul(FB_Proj, mul(viewMatrixInverse[count], mul(FB_InvView,  input[i].Position)));
            output.Append(element);
        }
        
        output.RestartStrip();
    }
}