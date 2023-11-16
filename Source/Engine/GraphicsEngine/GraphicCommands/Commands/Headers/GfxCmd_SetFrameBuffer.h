#pragma once 
#include <Engine/GraphicsEngine/GraphicCommands/GraphicCommands.h>

class GfxCmd_SetFrameBuffer : public GraphicCommandBase
{
private:
	Matrix myViewMatrix;
	Matrix myProjectionMatrix;
	Vector3f myPosition; 
	std::array<Vector4f,4> FB_FrustrumCorners = {Vector4f(1,0,0,0),Vector4f(-1,0,0,0),Vector4f(0,1,0,0),Vector4f(0,-1,0,0)};
	int RenderMode;
public:
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Transform& ref,int aRenderMode,const cCamera& camera);
	GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Matrix& ref,int aRenderMode);
	void Destroy() override {};
	void ExecuteAndDestroy() override;
};
