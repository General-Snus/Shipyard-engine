#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include "CameraComponent.h"
#include <Game/Modelviewer/Core/Modelviewer.h>
#include <Engine/GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h>
#include <Tools/Utilities/Input/InputHandler.hpp> 

cCamera::cCamera(const unsigned int anOwnerId) : Component(anOwnerId)
{

	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);

	const float fow = 3.14f * (85.0f / 180.0f);
	const float fowmdf = 1.0f / (tanf(fow / 2.0f));
	const float APRatio = 16.0f / 9.0f;
	const float farfield = 1500;
	const float nearField = .001f;

	myClipMatrix(1, 1) = fowmdf;
	myClipMatrix(2, 2) = APRatio * fowmdf;
	myClipMatrix(3, 3) = APRatio * fowmdf;
	myClipMatrix(3, 3) = farfield / (farfield - nearField);
	myClipMatrix(4, 3) = nearField * farfield / (farfield - nearField);
	myClipMatrix(3, 4) = 1;

	GetGameObject().AddComponent<cLight>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<cLight>().GetData<SpotLight>();
	pLight.lock()->Position = CU::Vector3<float>(0, 0, 0);
	pLight.lock()->Color = CU::Vector3<float>(1, 1, 1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = { 0,-1,0 };
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
	GetComponent<cLight>().BindDirectionToTransform(true);
}

cCamera::~cCamera()
{
}

void cCamera::Update()
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>(); 
	float aTimeDelta = CU::Timer::GetInstance().GetDeltaTime();

	//UpdatePositionVectors();
	const float mdf = 1000;
	const float rotationSpeed = 100;


	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::MOUSERBUTTON))
	{ 
		const Vector3f mouseDeltaVector = 
		{
			-static_cast<float>(CU::InputHandler::GetInstance().GetMousePositionDelta().y),
			-static_cast<float>(CU::InputHandler::GetInstance().GetMousePositionDelta().x),
			0.0f
		};
		myTransform.Rotate(mouseDeltaVector * rotationSpeed * 0.1f * CU::Timer::GetInstance().GetDeltaTime());
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::W))
	{
		myTransform.Move(myTransform.GetForward() * aTimeDelta * mdf);
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::S))
	{
		myTransform.Move(-myTransform.GetForward() * aTimeDelta * mdf);
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::D))
	{
		myTransform.Move(myTransform.GetRight() * aTimeDelta * mdf);
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::A))
	{
		myTransform.Move(-myTransform.GetRight() * aTimeDelta * mdf);
	}
	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::E))
	{
		myTransform.Rotate({ 0,rotationSpeed * aTimeDelta });
	}
	if(CU::InputHandler::GetInstance().IsKeyPressed((int)Keys::F))
	{
		GetComponent<cLight>().BindDirectionToTransform(!GetComponent<cLight>().GetIsBound());
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::Q))
	{
		myTransform.Rotate({ 0,-rotationSpeed * aTimeDelta });
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::SPACE))
	{
		myTransform.Move(myTransform.GetUp() * aTimeDelta * mdf);
	}

	if(CU::InputHandler::GetInstance().IsKeyHeld((int)Keys::SHIFT))
	{
		myTransform.Move(-myTransform.GetUp() * aTimeDelta * mdf);
	}
}

void cCamera::Render()
{
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_SetFrameBuffer>(myClipMatrix, this->GetGameObject().GetComponent<Transform>(), (int)ModelViewer::GetApplicationState().filter);
}

Vector3f cCamera::GetPointerDirection(const CU::Vector2<int> position)
{
	Vector4f viewPosition;
	RHI::DeviceSize size = RHI::GetDeviceSize();

	viewPosition.x = ((2.0f * position.x / size.Width) - 1);
	viewPosition /= myClipMatrix(1, 1);

	viewPosition.y = ((-2.0f * position.y / size.Height) - 1);
	viewPosition /= myClipMatrix(2, 2);

	viewPosition.z = 1;
	viewPosition.w = 0;

	const Matrix myTransform = GetGameObject().GetComponent<Transform>().GetTransform();
	const Vector4f out = viewPosition * Matrix::GetFastInverse(myTransform);
	return Vector3f(out.x, out.y, out.z);

}
Vector3f cCamera::GetPointerDirectionNDC(const Vector2f position) const
{
	position;
	throw std::exception("Not implemented");



}

void cCamera::SetCameraToFrameBuffer()
{
	GfxCmd_SetFrameBuffer(myClipMatrix,this->GetGameObject().GetComponent<Transform>(),(int)ModelViewer::GetApplicationState().filter).ExecuteAndDestroy(); 
}

CU::Vector4<float> cCamera::WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity)
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	//Get actuall world space coordinate
	CommonUtilities::Vector4<float> outPosition = { aEntity.x,aEntity.y,aEntity.z,1 };

	//Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.GetTransform());

	//Camera space -> ClipSpace
	outPosition = outPosition * myClipMatrix;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}
