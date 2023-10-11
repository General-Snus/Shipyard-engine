#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/MeshRenderer.h>

class GfxCmd_RenderMesh : public GraphicCommandBase
{
protected:
	std::vector<Element> myElementsData;
	std::vector<std::weak_ptr<Material>> myMaterials;
	Matrix myTransform;
	CU::Vector3<float> MinExtents; // 12 bytes 
	CU::Vector3<float> MaxExtents; // 12 bytes  

public:
	GfxCmd_RenderMesh(const RenderData& aData,const Matrix& aTransform); 
	void Destroy() override
	{
	};
	void ExecuteAndDestroy() override;
};
