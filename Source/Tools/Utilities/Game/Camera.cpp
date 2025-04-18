#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <cmath>
#include <Windows.h> 
#include "Camera.h"

MainCamera::MainCamera()
{
	myTransform(4,1) = 0;
	myTransform(4,2) = 0;
	myTransform(4,3) = 0;

	const float fow = 3.14f * (85.0f / 180.0f);
	const float fowmdf = 1.0f / (std::tan(fow / 2.0f));
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

inline MainCamera::MainCamera(Vector3f aPosition,Vector3f aRotation)
{
	aPosition; aRotation;

	Vector3f RightAxis = { 1,0,0 };
	Vector3f DownAxis = { 0,-1,0 };;
	Vector3f ForwardAxis = { 0,0,1 };;
	for (int i = 0; i < 3; i++)
	{
		myTransform(1,i + 1) = RightAxis[i];
		myTransform(2,i + 1) = DownAxis[i];
		myTransform(3,i + 1) = ForwardAxis[i];
	}

	myTransform(4,1) = aPosition.x;
	myTransform(4,2) = aPosition.y;
	myTransform(4,3) = aPosition.z;

	const float fow = 3.14f * (90.0f / 180.0f);
	const float fowmdf = 1.0f / (tan(fow / 2.0f));
	const float APRatio = 16.0f / 9.0f;
	const float fF = 100;
	const float nF = .02f;


	myClipMatrix(1,1) = fowmdf;
	myClipMatrix(2,2) = APRatio * fowmdf;
	myClipMatrix(3,3) = APRatio * fowmdf;
	myClipMatrix(3,3) = fF / (fF - nF);
	myClipMatrix(4,3) = nF * fF / (fF - nF);
	myClipMatrix(3,4) = 1;


}

MainCamera::~MainCamera()
{
}

void MainCamera::UpdatePositionVectors()
{
	for (int i = 1; i <= 3; i++)
	{
		myDirections[i - 1] = { myTransform(i,1),myTransform(i,2),myTransform(i,3) };
	}
}

void MainCamera::Update(float aTimeDelta)
{
	UpdatePositionVectors();
	const float mdf = 5;
	if (GetAsyncKeyState('W'))
	{
		for (int i = 1; i <= 3; i++)
		{
			myTransform(4,i) += mdf * aTimeDelta * myDirections.z[i - 1];
		}

	}

	if (GetAsyncKeyState('S'))
	{
		for (int i = 1; i <= 3; i++)
		{
			myTransform(4,i) += mdf * -aTimeDelta * myDirections.z[i - 1];
		}
	}

	if (GetAsyncKeyState('D'))
	{
		for (int i = 1; i <= 3; i++)
		{
			myTransform(4,i) += mdf * aTimeDelta * myDirections.x[i - 1];
		}
	}

	if (GetAsyncKeyState('A'))
	{

		for (int i = 1; i <= 3; i++)
		{
			myTransform(4,i) += mdf * -aTimeDelta * myDirections.x[i - 1];
		}
	}

	if (GetAsyncKeyState('E'))
	{
		myTransform = myTransform.CreateRotationAroundY(2.0f * aTimeDelta) * myTransform;
	}

	if (GetAsyncKeyState('Q'))
	{
		myTransform = myTransform.CreateRotationAroundY(-2.0f * aTimeDelta) * myTransform;
	}
}

Vector4f MainCamera::WoldSpaceToPostProjectionSpace(Vector3f aEntity) const
{
	//Get actuall world space coordinate
	Vector4f outPosition = { aEntity.x,aEntity.y,aEntity.z,1 };

	//Punkt -> CameraSpace
	outPosition = outPosition * myTransform.GetFastInverse(myTransform);

	//Camera space -> ClipSpace
	outPosition = outPosition * myClipMatrix;

	outPosition.x /= outPosition.w;
	outPosition.y /= outPosition.w;
	outPosition.z /= outPosition.w;

	return outPosition;
}

Matrix  MainCamera::GetTransform()
{
	return myTransform;
}
