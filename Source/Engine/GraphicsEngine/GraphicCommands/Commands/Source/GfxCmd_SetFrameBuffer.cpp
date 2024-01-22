#include <GraphicsEngine.pch.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Shaders/Registers.h>
#include <Tools/Optick/src/optick.h>
#include <Tools/Utilities/Game/Timer.h> 
#include "../Headers/GfxCmd_SetFrameBuffer.h"  
#include <Tools/Utilities/LinearAlgebra/Matrix4x4.hpp>
//Remember to invert the matrix
GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Matrix& ref,int aRenderMode,const std::array<Vector4f,4>& FrustrumCorners) :
	myViewMatrix(ref),
	myProjectionMatrix(ProjectionMatrix),
	myPosition({ ref(4,1),ref(4,2),ref(4,3) }),
	RenderMode(aRenderMode)
{
	FB_FrustrumCorners = FrustrumCorners;
}

void GfxCmd_SetFrameBuffer::ExecuteAndDestroy()
{
	OPTICK_EVENT();
	FrameBuffer& buffert = GetFrameBuffer();
	buffert.Data.ProjectionMatrix = myProjectionMatrix;
	buffert.Data.ViewMatrix = myViewMatrix;
	buffert.Data.Time = Timer::GetInstance().GetDeltaTime();
	buffert.Data.FB_RenderMode = RenderMode;

	buffert.Data.FB_CameraPosition[0] = myPosition.x;
	buffert.Data.FB_CameraPosition[1] = myPosition.y;
	buffert.Data.FB_CameraPosition[2] = myPosition.z;

	buffert.Data.FB_ScreenResolution[0] = static_cast<int>(RHI::GetDeviceSize().Width);
	buffert.Data.FB_ScreenResolution[1] = static_cast<int>(RHI::GetDeviceSize().Height);
	buffert.Data.FB_FrustrumCorners = FB_FrustrumCorners;

	RHI::SetConstantBuffer(PIPELINE_STAGE_VERTEX_SHADER | PIPELINE_STAGE_GEOMETERY_SHADER | PIPELINE_STAGE_PIXEL_SHADER,REG_FrameBuffer,buffert);
	RHI::UpdateConstantBufferData(buffert);
}