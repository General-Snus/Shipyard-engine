#include "AssetManager.pch.h"
#include "CameraComponent.h"

cCamera::cCamera(const unsigned int anOwnerId) : Component(anOwnerId)
{

	GetGameObject().AddComponent<Transform>();


	const float fow = 3.14f * (85.0f / 180.0f);
	const float fowmdf = 1.0f / (tanf(fow / 2.0f));
	const float APRatio = 16.0f / 9.0f;
	const float farfield = 1500;
	const float nearField = .001f;

	myClipMatrix(1,1) = fowmdf;
	myClipMatrix(2,2) = APRatio * fowmdf;
	myClipMatrix(3,3) = APRatio * fowmdf;
	myClipMatrix(3,3) = farfield / (farfield - nearField);
	myClipMatrix(4,3) = nearField * farfield / (farfield - nearField);
	myClipMatrix(3,4) = 1;
}

cCamera::~cCamera()
{
}

void cCamera::Update()
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	float aTimeDelta = CU::Timer::GetInstance().GetDeltaTime();
	//UpdatePositionVectors();
	const float mdf = 150;
	const float rotationSpeed = 100;
	if(GetAsyncKeyState('W'))
	{
		myTransform.Move(myTransform.GetForward() * aTimeDelta * mdf);
	}

	if(GetAsyncKeyState('S'))
	{
		myTransform.Move(-myTransform.GetForward() * aTimeDelta * mdf);
	}

	if(GetAsyncKeyState('D'))
	{
		myTransform.Move(myTransform.GetRight() * aTimeDelta * mdf);
	}

	if(GetAsyncKeyState('A'))
	{
		myTransform.Move(-myTransform.GetRight() * aTimeDelta * mdf);
	}
	if(GetAsyncKeyState('E'))
	{
		myTransform.Rotate({0,rotationSpeed * aTimeDelta});
	}

	if(GetAsyncKeyState('Q'))
	{
		myTransform.Rotate({0,-rotationSpeed * aTimeDelta});
	}

	if(GetAsyncKeyState(VK_SPACE))
	{
		myTransform.Move(myTransform.GetUp() * aTimeDelta * mdf);
	}

	if(GetAsyncKeyState(VK_LSHIFT))
	{
		myTransform.Move(-myTransform.GetUp() * aTimeDelta * mdf);
	}
}

void cCamera::Render()
{
	GraphicsEngine::Get().AddCommand<GfxCmd_SetFrameBuffer>(myClipMatrix,this->GetGameObject().GetComponent<Transform>());
}

CU::Vector4<float> cCamera::WoldSpaceToPostProjectionSpace(CommonUtilities::Vector3<float> aEntity)
{
	Transform& myTransform = this->GetGameObject().GetComponent<Transform>();

	//Get actuall world space coordinate
	CommonUtilities::Vector4<float> outPosition = {aEntity.x,aEntity.y,aEntity.z,1};

	//Punkt -> CameraSpace
	outPosition = outPosition * Matrix::GetFastInverse(myTransform.GetTransform());

	//Camera space -> ClipSpace
	outPosition = outPosition * myClipMatrix;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}
