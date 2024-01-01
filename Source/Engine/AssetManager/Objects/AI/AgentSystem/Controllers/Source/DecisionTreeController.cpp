#include "AssetManager.pch.h"
#include "../DecisionTreeController.h" 
#include <Engine/PersistentSystems/ArtificialInteligence/AICommands/AICommands.h>

DecisionTreeController::DecisionTreeController(DecisionTree decisionTree) : myTree(decisionTree)
{
}

DecisionTreeController::DecisionTreeController()
{
#if ActorDecicionTree
	int node = 0;	 //switching variables
	int node1 = 0;
	myTree.AddNodeAt(0,GeneralizedAICommands::IsDead); // nodes follow a triangle pyramid pattern of i++
	myTree.AddChildNodeAt(0,true,GeneralizedAICommands::DeathSpin);


	node = myTree.AddChildNodeAt(0,false,GeneralizedAICommands::IsHealthy); // or healing
	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::Retreat); // go and heal
	node = myTree.AddChildNodeAt(node,true,GeneralizedAICommands::IsTargetAlive);

	node1 = myTree.AddChildNodeAt(node,false,GeneralizedAICommands::IsFullyHealed);
	myTree.AddChildNodeAt(node1,true,GeneralizedAICommands::MoveFreely);
	myTree.AddChildNodeAt(node1,false,GeneralizedAICommands::Retreat); // go and heal

	node = myTree.AddChildNodeAt(node,true,GeneralizedAICommands::IsTargetInRange);
	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::MoveFreely);
	node = myTree.AddChildNodeAt(node,true,GeneralizedAICommands::IsTargetInSight);

	myTree.AddChildNodeAt(node,false,GeneralizedAICommands::AlignToTarget);
	myTree.AddChildNodeAt(node,true,GeneralizedAICommands::ShootAtTarget);

	//myTree.AddChildNodeAt(node,false,GeneralizedAICommands::MoveToward);



#endif
}


bool DecisionTreeController::Update(GameObject input)
{
	myTree.RunTree(input);

	auto& physicsComponent = input.GetComponent<cPhysics_Kinematic>();
	auto& transform = input.GetComponent<Transform>();

	//Corrects tree position
	Vector3f newPosition = SteeringBehaviour::SetPositionInBounds(transform.GetPosition(),50.0f);
	transform.SetPosition(newPosition);

	Vector3f normal;
	Vector3f ref = AIPollingManager::Get().GetStation<MultipleTargets_PollingStation>("Colliders")->GetClosestPositionOnCollider(transform.GetPosition(),normal);
	DebugDrawer::Get().AddDebugLine(transform.GetPosition(),ref,Vector3f(0.0f,1.0f,0.01f),0.01f);
	SteeringBehaviour::Separation(ref,&physicsComponent,transform.GetPosition());


	return true;
}

bool DecisionTreeController::ComponentRequirement(GameObject input)
{
	if(!input.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = input.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 5.0f;
		phy.ph_maxAcceleration = 10.0f;
	}

	if(!input.TryGetComponent<CombatComponent>())
	{
		input.AddComponent<CombatComponent>();
	}
	return true;
}

