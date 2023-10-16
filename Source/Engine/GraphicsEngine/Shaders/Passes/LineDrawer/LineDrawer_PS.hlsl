#include "LineDrawerStructs.hlsli"

LinePixelOutput main(LineVertexToPixel input)
{
    LinePixelOutput result;
    result.Color = input.Color;
	return result;
}