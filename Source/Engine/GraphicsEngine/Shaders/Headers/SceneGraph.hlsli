#include "ShaderStructs.hlsli"
#include "../Registers.h" 
#ifndef SCENEGRAPH
#define SCENEGRAPH 

struct Mesh
{
    uint bufferIdx;
    uint positionsOffset;
    uint uvsOffset;
    uint normalsOffset;
    uint tangentsOffset;
    uint indicesOffset;
    uint meshletOffset;
    uint meshletVerticesOffset;
    uint meshletTrianglesOffset;
    uint meshletCount;
};
#endif
