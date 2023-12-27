#include "../DecisionTreeController.h" 
#include "AssetManager.pch.h"

DecisionTreeController::DecisionTreeController(DecisionTree decicionTree) : myTree(decicionTree)
{
}

DecisionTreeController::DecisionTreeController()
{
#if ActorDecicionTree
	myTree.AddNodeAt(0,GeneralizedAICommands::IsDead);
	myTree.AddNodeAt(1,GeneralizedAICommands::IsHealthy);


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

