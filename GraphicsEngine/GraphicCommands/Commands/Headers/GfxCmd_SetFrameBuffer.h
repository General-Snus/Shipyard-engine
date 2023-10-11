#pragma once 
#include <GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_SetFrameBuffer : public GraphicCommandBase
{
private:
	Matrix myViewMatrix;
	Matrix myProjectionMatrix;
	Vector3f myPosition;
	float myDeltaTime;
	int RenderMode;
public:
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Transform& ref,int aRenderMode);
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Matrix& ref,int aRenderMode);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
