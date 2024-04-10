#pragma once 
#include <Engine/AssetManager/ComponentSystem/Components/MeshRenderer.h>
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_RenderMesh : public GraphicCommandBase
{
protected:
	const std::shared_ptr<RenderData> myRenderData;
	const Matrix  myTransform;
	Vector3f MinExtents; // 12 bytes 
	Vector3f MaxExtents; // 12 bytes  
	bool instanced;

public:
	GfxCmd_RenderMesh(const std::shared_ptr<RenderData>& aData,const Matrix& aTransform,bool instanced);
	void Destroy() override
	{
	};
	void ExecuteAndDestroy() override;
	~GfxCmd_RenderMesh() override = default;
};
