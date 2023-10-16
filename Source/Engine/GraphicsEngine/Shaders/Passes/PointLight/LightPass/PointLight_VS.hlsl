#include "../../../Headers/ShaderStructs.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
    return mul(OB_Transform, pos);
}