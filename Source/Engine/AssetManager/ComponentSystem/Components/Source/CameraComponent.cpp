#include "AssetManager.pch.h"

#include "../CameraComponent.h"
#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/Input/Input.hpp>

#include "Editor/Editor/Core/ApplicationState.h"
#include "Tools/Utilities/Input/EnumKeys.h"

Camera::Camera(const SY::UUID anOwnerId,GameObjectManager* aManager)
	: Component(anOwnerId,aManager),localTransform(0,nullptr) {
	localTransform.Init();
	if(!isOrtho) {
		const auto dxMatrix = XMMatrixPerspectiveFovLH(FowInRad(),APRatio(),farfield,nearField);
		m_Projection = Matrix(&dxMatrix);
	} else {
		const auto dxMatrix = XMMatrixOrthographicLH(fow,fow,farfield,nearField);
		m_Projection = Matrix(&dxMatrix);
	}

#ifdef Flashlight
	gameObject().AddComponent<Light>(eLightType::Spot);
	std::weak_ptr<SpotLight> pLight = GetComponent<Light>().GetData<SpotLight>();
	pLight.lock()->Position = Vector3f(0,0,0);
	pLight.lock()->Color = Vector3f(1,1,1);
	pLight.lock()->Power = 2000.0f * Kilo;
	pLight.lock()->Range = 1000;
	pLight.lock()->Direction = { 0, -1, 0 };
	pLight.lock()->InnerConeAngle = 10 * DEG_TO_RAD;
	pLight.lock()->OuterConeAngle = 45 * DEG_TO_RAD;
#endif
}

void Camera::Update() {
	OPTICK_EVENT();
	localTransform.Update();
	UpdateProjection();
	if(IsInControll) {
		EditorCameraControlls();
#ifdef Flashlight
		if(Input.GetInstance().IsKeyPressed((int)Keys::F)) {
			GetComponent<Light>().BindDirectionToTransform(!GetComponent<Light>().GetIsBound());
		}
#endif
	}
}

void Camera::EditorCameraControlls() {
	Transform& aTransform = localTransform;
	if(HasComponent<Transform>()) {
		aTransform = this->gameObject().GetComponent<Transform>();
	}

	const float           aTimeDelta = TimerInstance.getDeltaTime();
	Vector3f        mdf = cameraSpeed;
	float           rotationSpeed = 20000;
	constexpr float mousePower = 50.f;

	if(Input.IsKeyHeld(Keys::SHIFT)) {
		mdf = Vector3f();
	}

	if(Input.IsKeyHeld(Keys::LCONTROL)) {
		mdf = Vector3f();
		rotationSpeed = 0;
	}

	bool anyMouseKeyHeld = false;

	if(Input.IsKeyHeld(Keys::MBUTTON)) {
		anyMouseKeyHeld = true;
		const Vector3f movementUp = GlobalUp * Input.GetMousePositionDelta().y * mousePower;
		const Vector3f movementRight = aTransform.GetRight() * Input.GetMousePositionDelta().x * mousePower;
		aTransform.Move((movementUp + movementRight) * aTimeDelta * mdf);
	}

	if(Input.IsKeyHeld(Keys::MOUSERBUTTON)) {
		anyMouseKeyHeld = true;
		const Vector3f mouseDeltaVector = {
			std::abs(Input.GetMousePositionDelta().y) > 0.001f ? -Input.GetMousePositionDelta().y : 0.f,
			std::abs(Input.GetMousePositionDelta().x) > 0.001f ? Input.GetMousePositionDelta().x : 0.f, 0.0f
		};

		aTransform.Rotate(mouseDeltaVector * rotationSpeed * aTimeDelta);
	}

	if(Input.IsKeyHeld(Keys::MOUSELBUTTON)) {
		anyMouseKeyHeld = true;
		const Vector3f mouseDeltaVector = {
			0.0f, std::abs(Input.GetMousePositionDelta().x) > 0.001f ? Input.GetMousePositionDelta().x : 0.f, 0.0f
		};
		// Logger.Log(std::format("mouse delta x {}",mouseDeltaVector.y));
		aTransform.Rotate(mouseDeltaVector * rotationSpeed * aTimeDelta);

		Vector3f newForward = aTransform.GetForward() + aTransform.GetUp();
		newForward.y = 0;
		newForward.Normalize();
		aTransform.Move(newForward * Input.GetMousePositionDelta().y * mousePower * aTimeDelta * mdf);
	}

	if(Input.GetMouseWheelDelta() > Input.g_MouseWheelDeadZone) {
		if(anyMouseKeyHeld) {
			cameraSpeed = std::clamp(cameraSpeed * 1.1f,.5f,static_cast<float>(INT_MAX));
		} else {
			aTransform.Move(aTransform.GetForward() * 5.0f * aTimeDelta * mdf);
		}
	}
	if(Input.GetMouseWheelDelta() < -Input.g_MouseWheelDeadZone) {
		if(anyMouseKeyHeld) {
			cameraSpeed = std::clamp(cameraSpeed * 0.95f,.5f,static_cast<float>(INT_MAX));
		} else {
			aTransform.Move(-aTransform.GetForward() * 5.0f * aTimeDelta * mdf);
		}
	}
	if(anyMouseKeyHeld) {
		if(Input.IsKeyHeld(Keys::W)) {
			aTransform.Move(aTransform.GetForward() * aTimeDelta * mdf);
		}
		if(Input.IsKeyHeld(Keys::S)) {
			aTransform.Move(-aTransform.GetForward() * aTimeDelta * mdf);
		}
		if(Input.IsKeyHeld(Keys::D)) {
			aTransform.Move(aTransform.GetRight() * aTimeDelta * mdf);
		}
		if(Input.IsKeyHeld(Keys::A)) {
			aTransform.Move(-aTransform.GetRight() * aTimeDelta * mdf);
		}

		if(Input.IsKeyHeld(Keys::E)) {
			aTransform.Move(GlobalUp * aTimeDelta * mdf);
		}

		if(Input.IsKeyHeld(Keys::Q)) {
			aTransform.Move(-GlobalUp * aTimeDelta * mdf);
		}
	}
}

std::array<Vector4f,4> Camera::GetFrustrumCorners() const {
	constexpr float         farplane = 10000; // I dont use farplanes but some effect wants them anyway
	const float             halfHeight = farplane * tanf(0.25f * FowInRad());
	const float             halfWidth = APRatio() * halfHeight;
	std::array<Vector4f,4> corners;
	corners[0] = {-halfWidth, -halfHeight, farplane, 0.0f};
	corners[1] = {-halfWidth, halfHeight, farplane, 0.0f};
	corners[2] = {halfWidth, +halfHeight, farplane, 0.0f};
	corners[3] = {+halfWidth, -halfHeight, farplane, 0.0f};

	for(auto& i : corners) {
		i.Normalize();
	}

	return corners;
}

Vector3f Camera::GetPointerDirection(const Vector2f position) {
	Vector4f viewPosition;
	viewPosition.x = position.x;
	viewPosition.y = position.y;
	viewPosition.w = 1.0f;

	const Matrix   mvp = Matrix::Invert(ViewMatrix() * GetProjection());

	viewPosition.z = 0.0f;
	Vector4f outStart = viewPosition * mvp; 
	viewPosition.z = 1.0f;
	Vector4f outEnd = viewPosition * mvp;

	if(outEnd.w == 0 || outStart.w == 0) {
		return {0,0,0};
	}

	outEnd /= outEnd.w;
	outStart /= outStart.w;

	//This looks weird, reasoning is that i have inverted nearfar planes so its the opposite direction
	return (outStart - outEnd).xyz();
}

Vector3f Camera::GetPointerDirectionNDC(const Vector2f position) const {
	position;
	throw std::exception("Not implemented");
}

const Matrix& Camera::ViewMatrix() {
	m_CachedViewMatrix = Matrix::Invert(LocalTransform().WorldMatrix());
	return m_CachedViewMatrix;
}

FrameBuffer Camera::GetFrameBuffer() {
	OPTICK_EVENT();
	auto& transform = LocalTransform();
	FrameBuffer buffer;
	buffer.ProjectionMatrix = m_Projection;
	buffer.ViewMatrix = Matrix::GetFastInverse(transform.WorldMatrix());
	buffer.Time = TimerInstance.getDeltaTime();
	buffer.FB_RenderMode = static_cast<int>(filter);
	buffer.FB_CameraPosition = transform.GetPosition();
	buffer.FB_ScreenResolution = static_cast<Vector2ui>(resolution);
	// buffer.Data.FB_FrustrumCorners = { Vector4f(),Vector4f(),Vector4f(),Vector4f() };;
	return buffer;
}

Transform& Camera::LocalTransform() {
	if(HasComponent<Transform>()) {
		return GetComponent<Transform>();
	}
	return localTransform;
}

const Transform& Camera::LocalTransform() const {
	if(HasComponent<Transform>()) {
		return GetComponent<Transform>();
	}
	return localTransform;
}
void Camera::UpdateProjection() { 
	const auto dxMatrix = XMMatrixPerspectiveFovLH(FowInRad(),APRatio(),farfield,nearField);
	m_Projection = Matrix(&dxMatrix);
}
Vector4f Camera::WoldSpaceToPostProjectionSpace(Vector3f aEntity) {
	Transform& myTransform = this->gameObject().GetComponent<Transform>();

	// Get actuall world space coordinate
	Vector4f outPosition = { aEntity.x, aEntity.y, aEntity.z, 1 };

	// Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.LocalMatrix());

	// Camera space -> ClipSpace
	outPosition = outPosition * m_Projection;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}

bool Camera::InspectorView() {
	if(!Component::InspectorView()) {
		return false;
	}
	bool changed = Reflect<Camera>();

	static std::array<float,2> arr;
	if(isOrtho) {
		changed |= ImGui::DragFloat2("OrthoCamera Height and width",arr.data());
	}

	if(changed) {
		fow = std::clamp(fow,0.1f,360.f);
		farfield = std::clamp(farfield,nearField,std::numeric_limits<float>::max());
		nearField = std::clamp(nearField,0.000001f,farfield);
		if(!isOrtho) {
			const auto dxMatrix = XMMatrixPerspectiveFovLH(FowInRad(),APRatio(),farfield,nearField);
			m_Projection = Matrix(&dxMatrix);
		} else {
			for(auto& fl : arr) {
				fl = std::clamp(fl,0.0001f,std::numeric_limits<float>::max());
			}

			const auto dxMatrix = XMMatrixOrthographicLH(arr[0],arr[1],farfield,nearField);
			m_Projection = Matrix(&dxMatrix);
		}
	}

	return changed;
}
