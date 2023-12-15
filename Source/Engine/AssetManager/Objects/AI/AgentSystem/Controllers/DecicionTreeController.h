#pragma once
#include "Controller.h"
#include "../DecicionTree/DecicionTree.h"

class DecicionTreeController : public Controller
{
public:
	explicit DecicionTreeController(DecicionTree decicionTree);
	explicit DecicionTreeController();
	bool Update(GameObject input) override;

private:
	DecicionTree myTree;


	//DecicionTreeFunctions
private:
	//Decicion functions
	bool IsTargetInSight();
	bool IsTargetInRange();
	bool IsTargetAlive();
	bool IsDead();
	bool IsHealthy();
	bool IsFighting();
	bool IsWinning();

	//Action functions
	bool ShootAtTarget();
	bool Retreat();
	bool MoveToward();
	bool AlignToTarget();

	Transform* target;

};

