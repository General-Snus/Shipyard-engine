 #include "../../Headers/ParticleStruct.hlsli"
 #include "../../Headers/ShaderStructs.hlsli"

[maxvertexcount(4)]
void main(
	point ParticleVertexData input[1],
	inout TriangleStream<ParticleGeometryToPixel> output
)
{
	
	
	const float2 offsets[4] =
	{
	float2(-1.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(-1.0f, -1.0f),
	float2(1.0f, -1.0f)
	};
	const float2 uv[4] =
	{
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f)
	};
	
    const ParticleVertexData particle = input[0];
	
	for	(int i = 0; i < 4; ++i)
    {
        ParticleGeometryToPixel outputData;
        outputData.Position = mul(g_FrameBuffer.FB_InvView, particle.Position); // from world to view space
        outputData.Position.xy += offsets[i] * particle.Scale.xy; // Scale in space that is looking at camera, aka billboard
		outputData.Position = mul(g_FrameBuffer.FB_Proj, outputData.Position);  // apply projection
		
		outputData.Color = particle.Color;
		outputData.LifeTime = particle.LifeTime;
		outputData.UV = uv[i];
		outputData.Velocity = particle.Velocity;
		
        output.Append(outputData);
    }
	
	
	
}