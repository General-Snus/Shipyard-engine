#pragma once
#include "Controller.h"
#include "../NavMesh/NavMesh.h"


class PlayerController : public Controller
{
public:
	explicit PlayerController();
	SteeringOutput Update(const SteeringInput& aPosition) override;

private:
	void Input(const Vector3f& currentPosition);
	void Render();
	Vector3f end;
	NavMesh::Path myPath;
};