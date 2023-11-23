#include "PlayerController.h" 
#include "../../Input/InputHandler.hpp"

PlayerController::PlayerController()
{
	this->controllerType = eControllerType::player;
}

SteeringOutput PlayerController::Update(const  SteeringInput& input)
{
	SteeringOutput output;
	Input(input.position);
	Render();

	if(myPath.myPath.size())
	{
		if((input.position - myPath.myPath[0]).Length() < nodeDistance)
		{
			myPath.myPath.erase(myPath.myPath.begin());
		}
		if(myPath.myPath.size())
		{
			output.movement= (myPath.myPath[0] - input.position);
		}
		else
		{
			output.movement= Vector3f();
		}
	}
	return output;
}

void PlayerController::Input(const Vector3f& currentPosition)
{
	const Vector3f position = currentPosition;
	const Vector3f direction = {0.0f, 0.0f,1.0f};
	Vector3f hitPoint = {0.0f, 0.0f,1.0f};

	NavMesh::Polygon polygon;
	if(InputHandler::GetInstance().IsKeyPressed((int)Keys::MOUSELBUTTON))
	{
		bool hit{};
		//bool hit =  GetNavMesh().RayToNavMesh(position,direction,polygon,hitPoint);
		if(hit)
		{
			myPath.myPath.clear();
			end = hitPoint;
			//	myPath =  GetNavMesh().PathFind(currentPosition,end);
		}
	}
}

void PlayerController::Render()
{
	if(myPath.myPath.size())
	{
		//debugDrawer.DrawCircle({end.x,end.y},1);
		for(size_t i = 0; i < myPath.myPath.size() - 1; i++)
		{
			const Vector2f from = {myPath.myPath[i].x,myPath.myPath[i].y};
			const Vector2f to = {myPath.myPath[i + 1].x,myPath.myPath[i + 1].y};
			const Vector3f red = {1,0,0};

			//debugDrawer.DrawLine(from,to,red);
		}
	}
}
