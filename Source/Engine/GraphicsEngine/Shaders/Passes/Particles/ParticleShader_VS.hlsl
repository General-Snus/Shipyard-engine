 #include "../../Headers/ParticleStruct.hlsli"
 #include "../../Headers/ShaderStructs.hlsli"


ParticleVertexData main(ParticleVertexData input)
{
    ParticleVertexData data;
    const float4 localPosition = input.Position;
    const float4 worldPosition = mul(g_ObjectBuffer.OB_Transform, localPosition);
    
    data.Position = worldPosition;
    data.Color = input.Color;
    data.LifeTime = input.LifeTime;
    data.Scale = input.Scale;
    data.Velocity = input.Velocity; 
    
    return data;
}