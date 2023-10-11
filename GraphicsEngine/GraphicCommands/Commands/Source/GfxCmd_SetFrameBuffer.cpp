#include <GraphicsEngine.pch.h>
#include <ThirdParty/CU/Timer.h>
#include <AssetManager/Objects/Components/ComponentDerivatives/Transform.h>
#include "../Headers/GfxCmd_SetFrameBuffer.h"

GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Transform& ref,int aRenderMode)
{
	myViewMatrix = Matrix::GetFastInverse(ref.GetTransform());
	myProjectionMatrix = ProjectionMatrix;
	myDeltaTime = static_cast<float>(CU::Timer::GetInstance().GetTotalTime());
	myPosition = ref.GetPosition();
	RenderMode = aRenderMode;
}

//Remember to invert the matrix
GfxCmd_SetFrameBuffer::GfxCmd_SetFrameBuffer(const Matrix& ProjectionMatrix,const Matrix& ref,int aRenderMode)
{
	myViewMatrix = ref;
	myProjectionMatrix = ProjectionMatrix;
	myDeltaTime = static_cast<float>(CommonUtilities::Timer::GetInstance().GetTotalTime());
	myPosition = {ref(4,1),ref(4,2),ref(4,3)};
	RenderMode = aRenderMode;
}
void GfxCmd_SetFrameBuffer::ExecuteAndDestroy()
{
	FrameBuffer& buffert = GetFrameBuffer();
	buffert.Data.ProjectionMatrix = myProjectionMatrix;
	buffert.Data.ViewMatrix = myViewMatrix;
	buffert.Data.Time = myDeltaTime;
	buffert.Data.FB_RenderMode = RenderMode;

	buffert.Data.FB_CameraPosition[0] = myPosition.x;
	buffert.Data.FB_CameraPosition[1] = myPosition.y;
	buffert.Data.FB_CameraPosition[2] = myPosition.z;

	RHI::UpdateConstantBufferData(buffert);
}