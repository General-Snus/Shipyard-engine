#include "AssetManager.pch.h"
#include "../DecicionTreeController.h"

DecicionTreeController::DecicionTreeController(DecicionTree decicionTree) : myTree(decicionTree)
{
}

DecicionTreeController::DecicionTreeController()
{
#if ActorDecicionTree
    myTree.AddNodeAt(0,&DecicionTreeController::IsDead,this);
    myTree.AddNodeAt(1, &DecicionTreeController::IsHealthy,this) ;
    std::function<bool(void)> function2 = std::bind(&DecicionTreeController::IsHealthy,this);
#endif
}

bool DecicionTreeController::Update(GameObject input)
{
    input;
    myTree.RunTree();

    return false;
}

bool DecicionTreeController::IsTargetInSight()
{
    return false;
}

bool DecicionTreeController::IsTargetInRange()
{
    return false;
}

bool DecicionTreeController::IsTargetAlive()
{
    return false;
}

bool DecicionTreeController::IsDead()
{
    return false;
}

bool DecicionTreeController::IsHealthy()
{
    return false;
}

bool DecicionTreeController::IsFighting()
{
    return false;
}

bool DecicionTreeController::IsWinning()
{
    return false;
}

bool DecicionTreeController::ShootAtTarget()
{
    return false;
}

bool DecicionTreeController::Retreat()
{
    return false;
}

bool DecicionTreeController::MoveToward()
{
    return false;
}

bool DecicionTreeController::AlignToTarget()
{
    return false;
}
