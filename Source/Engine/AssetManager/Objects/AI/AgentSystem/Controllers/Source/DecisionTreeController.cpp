#include "AssetManager.pch.h"
#include "../DecisionTreeController.h" 

DecisionTreeController::DecisionTreeController(DecisionTree decicionTree) : myTree(decicionTree)
{
}

DecisionTreeController::DecisionTreeController()
{
#if ActorDecicionTree
	int node = 0;
	myTree.AddNodeAt(0,GeneralizedAICommands::IsDead);

	node = myTree.AddChildNodeAt(0,false,GeneralizedAICommands::IsHealthy);
	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::Retreat);
	node = myTree.AddChildNodeAt(node,true,GeneralizedAICommands::IsTargetInRange);

	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::MoveToward);
	node = myTree.AddChildNodeAt(node,true,GeneralizedAICommands::IsTargetInSight);

	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::AlignToTarget);
	myTree.AddChildNodeAt(node,true,GeneralizedAICommands::ShootAtTarget);
#endif
}

bool DecisionTreeController::Update(GameObject input)
{
	myTree.RunTree(input);
	return false;
}

bool DecisionTreeController::ComponentRequirement(GameObject input)
{
	if(!input.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = input.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 4.5f;
		phy.ph_maxAcceleration = 5.0f;
	}
	return true;
}

