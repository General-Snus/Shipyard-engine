#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <Engine/AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>

class GfxCmd_RenderMesh : public GraphicCommandBase
{
protected:
	RenderData* myRenderData;
	Matrix myTransform;
	CU::Vector3<float> MinExtents; // 12 bytes 
	CU::Vector3<float> MaxExtents; // 12 bytes  
	bool instanced;

public:
	GfxCmd_RenderMesh(RenderData* aData,const Matrix& aTransform,bool instanced);
	void Destroy() override
	{
	};
	void ExecuteAndDestroy() override;
};
