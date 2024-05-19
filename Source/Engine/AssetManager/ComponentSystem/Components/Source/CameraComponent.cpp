#include "AssetManager.pch.h"

#include <DirectXMath.h>
#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/Input/Input.hpp> 
#include "../CameraComponent.h"

#include <ResourceUploadBatch.h>

#include "DirectX/Shipyard/CommandList.h"
#include "DirectX/Shipyard/GPU.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/Utilities/Input/EnumKeys.h"

cCamera::cCamera(const unsigned int anOwnerId) : Component(anOwnerId)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false);


	if (!mySettings.isOrtho)
	{
		const auto dxMatrix = XMMatrixPerspectiveFovLH(mySettings.fow,mySettings.APRatio,mySettings.farfield,
			mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
	}
	else
	{
		const auto dxMatrix = XMMatrixOrthographicLH(40,40,1000000,mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
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
		const auto dxMatrix = XMMatrixPerspectiveFovLH(mySettings.fow,mySettings.APRatio,mySettings.farfield,
			mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
	}
	else
	{
		const auto dxMatrix = XMMatrixOrthographicLH(40,40,1000000,mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
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

	if (!mySettings.IsInControll)
	{
		return;
	}



	Transform& aTransform = this->GetGameObject().GetComponent<Transform>();
	float aTimeDelta = Timer::GetInstance().GetDeltaTime();
	const float mdf = cameraSpeed;
	const float rotationSpeed = 20000;


	myScreenSize = Editor::GetViewportResolution();

	if (Input::IsKeyHeld((int)Keys::UP))
	{
		cameraSpeed = cameraSpeed * 1.01f;
	}
	if (Input::IsKeyHeld((int)Keys::DOWN))
	{
		cameraSpeed = std::clamp(cameraSpeed * 0.99f,.5f,(float)INT_MAX);
	}

	if (Input::IsKeyHeld((int)Keys::MOUSERBUTTON))
	{
		const Vector3f mouseDeltaVector =
		{
			static_cast<float>(Input::GetMousePositionDelta().y),
			-static_cast<float>(Input::GetMousePositionDelta().x),
			0.0f
		};
		aTransform.Rotate(mouseDeltaVector * rotationSpeed * Timer::GetInstance().GetDeltaTime());
	}

	if (Input::IsKeyHeld((int)Keys::W))
	{
		aTransform.Move(aTransform.GetForward() * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld((int)Keys::S))
	{
		aTransform.Move(-aTransform.GetForward() * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld((int)Keys::D))
	{
		aTransform.Move(aTransform.GetRight() * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld((int)Keys::A))
	{
		aTransform.Move(-aTransform.GetRight() * aTimeDelta * mdf);
	}
	if (Input::IsKeyHeld((int)Keys::E))
	{
		aTransform.Rotate({ 0,200.f * aTimeDelta });
	}
#ifdef Flashlight
	if (Input::GetInstance().IsKeyPressed((int)Keys::F))
	{
		GetComponent<cLight>().BindDirectionToTransform(!GetComponent<cLight>().GetIsBound());
	}
#endif
	if (Input::IsKeyHeld((int)Keys::Q))
	{
		aTransform.Rotate({ 0,-200.f * aTimeDelta });
	}

	if (Input::IsKeyHeld((int)Keys::SPACE))
	{
		aTransform.Move(aTransform.GetUp() * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld((int)Keys::SHIFT))
	{
		aTransform.Move(-aTransform.GetUp() * aTimeDelta * mdf);
	}
}

void cCamera::Render()
{
}

void cCamera::SetSettings(const CameraSettings& settings)
{
	mySettings = settings;

	if (!mySettings.isOrtho)
	{
		const auto dxMatrix = XMMatrixPerspectiveFovLH(mySettings.fow,mySettings.APRatio,mySettings.farfield,
			mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
	}
	else
	{
		const auto dxMatrix = XMMatrixOrthographicLH(40,40,1000000,mySettings.nearField);
		m_Projection = Matrix(&dxMatrix);
	}
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
	viewPosition /= m_Projection(1,1);

	viewPosition.y = ((-2.0f * position.y / size.y) - 1);
	viewPosition /= m_Projection(2,2);

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

FrameBuffer cCamera::GetFrameBuffer()
{
	OPTICK_EVENT();
	auto& transform = GetComponent<Transform>();
	FrameBuffer buffer;
	buffer.ProjectionMatrix = m_Projection;
	buffer.ViewMatrix = Matrix::GetFastInverse(transform.GetTransform());
	buffer.Time = Timer::GetInstance().GetDeltaTime();
	buffer.FB_RenderMode = static_cast<int>(ApplicationState::filter);
	buffer.FB_CameraPosition = transform.GetPosition();
	buffer.FB_ScreenResolution = Editor::Get().GetViewportResolution();
	//buffer.Data.FB_FrustrumCorners = { Vector4f(),Vector4f(),Vector4f(),Vector4f() };;
	return buffer;
}

Vector4f cCamera::WoldSpaceToPostProjectionSpace(Vector3f aEntity)
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	//Get actuall world space coordinate
	Vector4f outPosition = { aEntity.x,aEntity.y,aEntity.z,1 };

	//Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.GetTransform());

	//Camera space -> ClipSpace
	outPosition = outPosition * m_Projection;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}

void cCamera::InspectorView()
{
	ImGui::Text("cCamera");
}