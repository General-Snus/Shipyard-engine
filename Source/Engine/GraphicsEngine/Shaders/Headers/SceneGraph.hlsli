#ifndef SCENEGRAPH
#define SCENEGRAPH
#include "ShaderStructs.hlsli"


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
Mesh GetMeshData(uint meshIdx)
{
    StructuredBuffer<Mesh> meshes = ResourceDescriptorHeap[meshIdx];
    return meshes[meshIdx];
}
#endif
