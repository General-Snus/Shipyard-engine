#include "Engine/AssetManager/AssetManager.pch.h"

#include <DirectXMath.h>
#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/Input/Input.hpp> 
#include "../CameraComponent.h"

#include <ResourceUploadBatch.h>

#include "DirectX/Shipyard/CommandList.h"
#include "DirectX/Shipyard/GPU.h"
#include "Editor/Editor/Core/ApplicationState.h"
#include "Engine/GraphicsEngine/GraphicsEngine.h"
#include "Tools/Utilities/Input/EnumKeys.h"

cCamera::cCamera(const SY::UUID anOwnerId,GameObjectManager* aManager) : Component(anOwnerId,aManager)
{
	GetGameObject().AddComponent<Transform>();
	GetGameObject().GetComponent<Transform>().SetGizmo(false); 

	if (!isOrtho)
	{
		const auto dxMatrix = XMMatrixPerspectiveFovLH(FowInRad(),APRatio(),farfield,nearField);
		m_Projection = Matrix(&dxMatrix);
	}
	else
	{
		const auto dxMatrix = XMMatrixOrthographicLH(fow,fow,farfield,nearField);
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
	fow = std::clamp(fow,0.1f,360.f);

	if (!IsInControll)
	{
		return;
	}

	Transform& aTransform = this->GetGameObject().GetComponent<Transform>();
	float aTimeDelta = Timer::GetDeltaTime();
	Vector3f mdf = cameraSpeed;
	float rotationSpeed = 20000;
	const float mousePower = 50.f;


	if (Input::IsKeyHeld(Keys::SHIFT))
	{
		mdf = Vector3f();
	}

	if (Input::IsKeyHeld(Keys::LCONTROL))
	{
		mdf = Vector3f();
		rotationSpeed = 0;
	}

	bool anyMouseKeyHeld = false;

	if (Input::IsKeyHeld(Keys::MBUTTON))
	{
		anyMouseKeyHeld = true;
		const Vector3f movementUp = GlobalUp * Input::GetMousePositionDelta().y * mousePower;
		const Vector3f movementRight = aTransform.GetRight() * Input::GetMousePositionDelta().x * mousePower;
		aTransform.Move((movementUp + movementRight) * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld(Keys::MOUSERBUTTON))
	{
		anyMouseKeyHeld = true;
		const Vector3f mouseDeltaVector =
		{
			std::abs(Input::GetMousePositionDelta().y) > 0.001f ? Input::GetMousePositionDelta().y : 0.f,
			std::abs(Input::GetMousePositionDelta().x) > 0.001f ? -Input::GetMousePositionDelta().x : 0.f,
			0.0f
		};

		aTransform.Rotate(mouseDeltaVector * rotationSpeed * aTimeDelta);
	}

	if (Input::IsKeyHeld(Keys::MOUSELBUTTON))
	{
		anyMouseKeyHeld = true;
		const Vector3f mouseDeltaVector =
		{
			0.0f,
			std::abs(Input::GetMousePositionDelta().x) > 0.001f ? Input::GetMousePositionDelta().x : 0.f,
			0.0f
		};
		//Logger::Log(std::format("mouse delta x {}",mouseDeltaVector.y));
		aTransform.Rotate(mouseDeltaVector * rotationSpeed * aTimeDelta);

		Vector3f newForward = aTransform.GetForward() + aTransform.GetUp();
		newForward.y = 0;
		newForward.Normalize();
		aTransform.Move(newForward * Input::GetMousePositionDelta().y * mousePower * aTimeDelta * mdf);
	}

	if (Input::GetMouseWheelDelta() > Input::g_MouseWheelDeadZone)
	{
		if (anyMouseKeyHeld)
		{
			cameraSpeed = std::clamp(cameraSpeed * 1.05f,.5f,static_cast<float>(INT_MAX));
		}
		else
		{

			aTransform.Move(aTransform.GetForward() * 10.0f * aTimeDelta * mdf);
		}
	}
	if (Input::GetMouseWheelDelta() < -Input::g_MouseWheelDeadZone)
	{
		if (anyMouseKeyHeld)
		{
			cameraSpeed = std::clamp(cameraSpeed * 0.95f,.5f,static_cast<float>(INT_MAX));
		}
		else
		{
			aTransform.Move(-aTransform.GetForward()*10.0f * aTimeDelta * mdf);
		}
	}

	if (Input::IsKeyHeld(Keys::W))
	{
		aTransform.Move(aTransform.GetForward() * aTimeDelta * mdf);
	}
	if (Input::IsKeyHeld(Keys::S))
	{
		aTransform.Move(-aTransform.GetForward() * aTimeDelta * mdf);
	}
	if (Input::IsKeyHeld(Keys::D))
	{
		aTransform.Move(aTransform.GetRight() * aTimeDelta * mdf);
	}
	if (Input::IsKeyHeld(Keys::A))
	{
		aTransform.Move(-aTransform.GetRight() * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld(Keys::E))
	{
		aTransform.Move(GlobalUp * aTimeDelta * mdf);
	}

	if (Input::IsKeyHeld(Keys::Q))
	{
		aTransform.Move(-GlobalUp * aTimeDelta * mdf);
	}
#ifdef Flashlight
	if (Input::GetInstance().IsKeyPressed((int)Keys::F))
	{
		GetComponent<cLight>().BindDirectionToTransform(!GetComponent<cLight>().GetIsBound());
	}
#endif
}

void cCamera::Render()
{
} 

std::array<Vector4f,4> cCamera::GetFrustrumCorners() const
{
	const float farplane = 10000; // I dont use farplanes but some effect wants them anyway 
	const float halfHeight = farplane * tanf(0.25f * FowInRad());
	const float halfWidth = APRatio() * halfHeight;
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
	auto dxMatrix = XMMatrixPerspectiveFovLH(FowInRad(),APRatio(),farfield,nearField);
	buffer.ProjectionMatrix = Matrix(&dxMatrix);
	buffer.ViewMatrix = Matrix::GetFastInverse(transform.GetTransform());
	buffer.Time = Timer::GetDeltaTime();
	buffer.FB_RenderMode = static_cast<int>(ApplicationState::filter);
	buffer.FB_CameraPosition = transform.GetPosition();
	buffer.FB_ScreenResolution = static_cast<Vector2ui>(resolution);
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

bool cCamera::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<cCamera>();  

	return true;
}