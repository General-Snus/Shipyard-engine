#include "AssetManager.pch.h"

#include <DirectXMath.h>
#include <Editor/Editor/Core/Editor.h>
#include <Engine/GraphicsEngine/GraphicCommands/Commands/Headers/GfxCmd_SetFrameBuffer.h> 
#include <Tools/Utilities/Input/InputHandler.hpp> 
#include "../CameraComponent.h"

#include "DirectX/Shipyard/CommandList.h"
#include "Tools/Utilities/Input/EnumKeys.h"

cCamera::cCamera(const unsigned int anOwnerId) : Component(anOwnerId)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);


	if (!mySettings.isOrtho)
	{
		myClipMatrix = DirectX::XMMatrixPerspectiveFovLH(mySettings.fow,mySettings.APRatio,mySettings.farfield,mySettings.nearField);
	}
	else
	{
		myClipMatrix = DirectX::XMMatrixOrthographicLH(40,40,1000000,mySettings.nearField);
	}

#ifdef Flashlight
	GetGameObject().AddComponent<cLight>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<cLight>().GetData<SpotLight>();
	pLight.lock()->Position = Vector3f(0,0,0);
	pLight.lock()->Color = Vector3f(1,1,1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = { 0,-1,0 };
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
	GetComponent<cLight>().BindDirectionToTransform(true);
#endif
}

cCamera::cCamera(const unsigned int anOwnerId,const CameraSettings& settings) : Component(anOwnerId)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);
	mySettings = settings;

	if (!mySettings.isOrtho)
	{
		myClipMatrix = DirectX::XMMatrixPerspectiveFovLH(mySettings.fow,mySettings.APRatio,mySettings.farfield,mySettings.nearField);
	}
	else
	{
		myClipMatrix = DirectX::XMMatrixOrthographicLH(40,40,1000000,mySettings.nearField);
	}

#ifdef Flashlight
	GetGameObject().AddComponent<cLight>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<cLight>().GetData<SpotLight>();
	pLight.lock()->Position = Vector3f(0,0,0);
	pLight.lock()->Color = Vector3f(1,1,1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = { 0,-1,0 };
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
	GetComponent<cLight>().BindDirectionToTransform(true);
#endif
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
	const float mdf = cameraSpeed;
	const float rotationSpeed = 20000;
	myScreenSize = Editor::GetViewportResolution();

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::UP))
	{
		cameraSpeed = cameraSpeed * 1.01f;
	}
	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::DOWN))
	{
		cameraSpeed = std::clamp(cameraSpeed * 0.99f,.5f,(float)INT_MAX);
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::MOUSERBUTTON))
	{
		const Vector3f mouseDeltaVector =
		{
			static_cast<float>(InputHandler::GetInstance().GetMousePositionDelta().y),
			-static_cast<float>(InputHandler::GetInstance().GetMousePositionDelta().x),
			0.0f
		};
		myTransform.Rotate(mouseDeltaVector * rotationSpeed * Timer::GetInstance().GetDeltaTime());
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::W))
	{
		myTransform.Move(myTransform.GetForward() * aTimeDelta * mdf);
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::S))
	{
		myTransform.Move(-myTransform.GetForward() * aTimeDelta * mdf);
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::D))
	{
		myTransform.Move(myTransform.GetRight() * aTimeDelta * mdf);
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::A))
	{
		myTransform.Move(-myTransform.GetRight() * aTimeDelta * mdf);
	}
	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::E))
	{
		myTransform.Rotate({ 0,200.f * aTimeDelta });
	}
#ifdef Flashlight
	if (InputHandler::GetInstance().IsKeyPressed((int)Keys::F))
	{
		GetComponent<cLight>().BindDirectionToTransform(!GetComponent<cLight>().GetIsBound());
	}
#endif
	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::Q))
	{
		myTransform.Rotate({ 0,-200.f * aTimeDelta });
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::SPACE))
	{
		myTransform.Move(myTransform.GetUp() * aTimeDelta * mdf);
	}

	if (InputHandler::GetInstance().IsKeyHeld((int)Keys::SHIFT))
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
	const float farplane = 10000; // I dont use farplanes but some effect wants them anyway
	const float aspectRatio = std::bit_cast<float>(myScreenSize.x) / std::bit_cast<float>(myScreenSize.y);
	const float halfHeight = farplane * tanf(0.25f * mySettings.fow);
	const float halfWidth = aspectRatio * halfHeight;
	std::array<Vector4f,4> corners;
	corners[0] = { -halfWidth, -halfHeight, farplane, 0.0f };
	corners[1] = { -halfWidth,	halfHeight, farplane, 0.0f };
	corners[2] = { halfWidth, +halfHeight, farplane, 0.0f };
	corners[3] = { +halfWidth, -halfHeight, farplane, 0.0f };

	for (auto& i : corners)
	{
		i.Normalize();
	}

	return corners;
}
Vector3f cCamera::GetPointerDirection(const Vector2<int> position)
{
	Vector4f viewPosition;
	auto size = Editor::GetViewportResolution();

	viewPosition.x = ((2.0f * position.x / size.x) - 1);
	viewPosition /= myClipMatrix(1,1);

	viewPosition.y = ((-2.0f * position.y / size.y) - 1);
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

void cCamera::SetCameraToFrameBuffer(DxCommandList& commandList)
{
	OPTICK_EVENT();
	auto& transform = GetComponent<Transform>();
	FrameBuffer buffer = FrameBuffer();
	buffer.Data.ProjectionMatrix = myClipMatrix;
	buffer.Data.ViewMatrix = Matrix::GetFastInverse(transform.GetTransform());
	buffer.Data.Time = Timer::GetInstance().GetDeltaTime();
	buffer.Data.FB_RenderMode = (int)Editor::GetApplicationState().filter;

	buffer.Data.FB_CameraPosition = transform.GetPosition();


	commandList->SetGraphicsRoot32BitConstants(REG_FrameBuffer,1,&buffer,0);
	//GfxCmd_SetFrameBuffer(
	//	myClipMatrix,
	//	),
	//	, //TODO scene again
	//	GetFrustrumCorners()
	//	).ExecuteAndDestroy();
}

Vector4f cCamera::WoldSpaceToPostProjectionSpace(Vector3f aEntity)
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	//Get actuall world space coordinate
	Vector4f outPosition = { aEntity.x,aEntity.y,aEntity.z,1 };

	//Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.GetTransform());

	//Camera space -> ClipSpace
	outPosition = outPosition * myClipMatrix;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}