#include "AssetManager.pch.h"
#include <Engine/GraphicsEngine/GraphicsEngine.pch.h>
#include "CameraComponent.h"
#include <Editor/Editor/Core/Editor.h>
#include <Engine/GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h>
#include <Tools/Utilities/Input/InputHandler.hpp> 

cCamera::cCamera(const unsigned int anOwnerId) : Component(anOwnerId)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);


	myClipMatrix(1,1) = mySettings.fowmdf;
	myClipMatrix(2,2) = mySettings.APRatio * mySettings.fowmdf;
	myClipMatrix(3,3) = mySettings.APRatio * mySettings.fowmdf;
	myClipMatrix(3,3) = mySettings.farfield / (mySettings.farfield - mySettings.nearField);
	myClipMatrix(4,3) = mySettings.nearField * mySettings.farfield / (mySettings.farfield - mySettings.nearField);
	myClipMatrix(3,4) = 1;

	GetGameObject().AddComponent<cLight>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<cLight>().GetData<SpotLight>();
	pLight.lock()->Position =  Vector3f(0,0,0);
	pLight.lock()->Color =  Vector3f(1,1,1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = {0,-1,0};
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
	GetComponent<cLight>().BindDirectionToTransform(true);
}

cCamera::cCamera(const unsigned int anOwnerId,CameraSettings settings) : Component(anOwnerId)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);

	myClipMatrix(1,1) = settings.fowmdf;
	myClipMatrix(2,2) = settings.APRatio * settings.fowmdf;
	myClipMatrix(3,3) = settings.APRatio * settings.fowmdf;
	myClipMatrix(3,3) = settings.farfield / (settings.farfield - settings.nearField);
	myClipMatrix(4,3) = settings.nearField * settings.farfield / (settings.farfield - settings.nearField);
	myClipMatrix(3,4) = 1;


	mySettings = settings;
	GetGameObject().AddComponent<cLight>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<cLight>().GetData<SpotLight>();
	pLight.lock()->Position =  Vector3f(0,0,0);
	pLight.lock()->Color =  Vector3f(1,1,1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = {0,-1,0};
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
	GetComponent<cLight>().BindDirectionToTransform(true);
}

cCamera::~cCamera()
{
}

void cCamera::Update()
{
	OPTICK_EVENT();
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();
	float aTimeDelta = Timer::GetInstance().GetDeltaTime();

	//UpdatePositionVectors();
	const float mdf = 1000;
	const float rotationSpeed = 100;
	myScreenSize =  Vector2<int>(RHI::GetDeviceSize().Width,RHI::GetDeviceSize().Height);

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::MOUSERBUTTON))
	{
		const Vector3f mouseDeltaVector =
		{
			-static_cast<float>(InputHandler::GetInstance().GetMousePositionDelta().y),
			-static_cast<float>(InputHandler::GetInstance().GetMousePositionDelta().x),
			0.0f
		};
		myTransform.Rotate(mouseDeltaVector * rotationSpeed * 0.1f * Timer::GetInstance().GetDeltaTime());
	}

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::W))
	{
		myTransform.Move(myTransform.GetForward() * aTimeDelta * mdf);
	}

	if( InputHandler::GetInstance().IsKeyHeld((int)Keys::S))
	{
		myTransform.Move(-myTransform.GetForward() * aTimeDelta * mdf);
	}

	if( InputHandler::GetInstance().IsKeyHeld((int)Keys::D))
	{
		myTransform.Move(myTransform.GetRight() * aTimeDelta * mdf);
	}

	if( InputHandler::GetInstance().IsKeyHeld((int)Keys::A))
	{
		myTransform.Move(-myTransform.GetRight() * aTimeDelta * mdf);
	}
	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::E))
	{
		myTransform.Rotate({0,rotationSpeed * aTimeDelta});
	}
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::F))
	{
		GetComponent<cLight>().BindDirectionToTransform(!GetComponent<cLight>().GetIsBound());
	}

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::Q))
	{
		myTransform.Rotate({0,-rotationSpeed * aTimeDelta});
	}

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::SPACE))
	{
		myTransform.Move(myTransform.GetUp() * aTimeDelta * mdf);
	}

	if(InputHandler::GetInstance().IsKeyHeld((int)Keys::SHIFT))
	{
		myTransform.Move(-myTransform.GetUp() * aTimeDelta * mdf);
	}
}

void cCamera::Render()
{
	//GraphicsEngine::Get().DeferredCommand<GfxCmd_SetFrameBuffer>(myClipMatrix, this->GetGameObject().GetComponent<Transform>(), (int)ModelViewer::GetApplicationState().filter);
}
std::array<Vector4f,4> cCamera::GetFrustrumCorners() const
{
	const float aspectRatio = std::bit_cast<float>(myScreenSize.x) / std::bit_cast<float>(myScreenSize.y);
	const float halfHeight = mySettings.farfield * tanf(0.25f * mySettings.fow);
	const float halfWidth = aspectRatio * halfHeight;
	std::array<Vector4f,4> corners;
	corners[0] = {-halfWidth, -halfHeight, mySettings.farfield, 0.0f};
	corners[1] = {-halfWidth,	halfHeight, mySettings.farfield, 0.0f};
	corners[2] = {halfWidth, +halfHeight, mySettings.farfield, 0.0f};
	corners[3] = {+halfWidth, -halfHeight, mySettings.farfield, 0.0f};

	return corners;
}
Vector3f cCamera::GetPointerDirection(const Vector2<int> position)
{
	Vector4f viewPosition;
	RHI::DeviceSize size = RHI::GetDeviceSize();

	viewPosition.x = ((2.0f * position.x / size.Width) - 1);
	viewPosition /= myClipMatrix(1,1);

	viewPosition.y = ((-2.0f * position.y / size.Height) - 1);
	viewPosition /= myClipMatrix(2,2);

	viewPosition.z = 1;
	viewPosition.w = 0;

	const Matrix myTransform = GetGameObject().GetComponent<Transform>().GetTransform();
	const Vector4f out = viewPosition * Matrix::GetFastInverse(myTransform);
	return Vector3f(out.x,out.y,out.z);

}
Vector3f cCamera::GetPointerDirectionNDC(const Vector2<int> position) const
{
	position;
	throw std::exception("Not implemented");



}

void cCamera::SetCameraToFrameBuffer()
{
	OPTICK_EVENT();
	GfxCmd_SetFrameBuffer(
		myClipMatrix,
		this->GetGameObject().GetComponent<Transform>(),
		0,//(int)Editor::GetApplicationState().filter, //TODO scene again
		*this
	).ExecuteAndDestroy();
}

Vector4f cCamera::WoldSpaceToPostProjectionSpace(Vector3f aEntity)
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	//Get actuall world space coordinate
	Vector4f outPosition = {aEntity.x,aEntity.y,aEntity.z,1};

	//Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.GetTransform());

	//Camera space -> ClipSpace
	outPosition = outPosition * myClipMatrix;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}