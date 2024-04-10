#include <Editor/Editor/Defines.h>
#include <Engine/AssetManager/AssetManager.pch.h>
#include <Engine/PersistentSystems/ArtificialInteligence/AICommands/AICommands.h>
#include "../DecisionTreeController.h" 

DecisionTreeController::DecisionTreeController(const DecisionTree& decisionTree) : myTree(decisionTree)
{
}

DecisionTreeController::DecisionTreeController()
{
#if ActorDecicionTree


	//Tack för bra kommentarer Simon, mycket klarare med ordentliga variabelnamn

	const int EntryNode = 0;
	myTree.AddNodeAt(EntryNode,GeneralizedAICommands::IsDead); // nodes follow a triangle pyramid pattern of i++
	myTree.AddChildNodeAt(EntryNode,true,GeneralizedAICommands::DeathSpin);


	const int isHealthy = myTree.AddChildNodeAt(EntryNode,false,GeneralizedAICommands::IsHealthy); // or healing
	myTree.AddChildNodeAt(isHealthy,false,GeneralizedAICommands::Retreat); // go and heal
	const int isTargetAlive = myTree.AddChildNodeAt(isHealthy,true,GeneralizedAICommands::IsTargetAlive);

	const int isFullyHealed = myTree.AddChildNodeAt(isTargetAlive,false,GeneralizedAICommands::IsFullyHealed);
	myTree.AddChildNodeAt(isFullyHealed,true,GeneralizedAICommands::MoveFreely);
	myTree.AddChildNodeAt(isFullyHealed,false,GeneralizedAICommands::Retreat); // go and heal

	const int isTargetInRange = myTree.AddChildNodeAt(isTargetAlive,true,GeneralizedAICommands::IsTargetInRange);
	myTree.AddChildNodeAt(isTargetInRange,false,GeneralizedAICommands::MoveFreely);
	const int isTargetInSight = myTree.AddChildNodeAt(isTargetInRange,true,GeneralizedAICommands::IsTargetInSight);

	myTree.AddChildNodeAt(isTargetInSight,false,GeneralizedAICommands::AlignToTarget);
	myTree.AddChildNodeAt(isTargetInSight,true,GeneralizedAICommands::ShootAtTarget);
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
	if (!input.TryGetComponent<cPhysics_Kinematic>())
	{
		auto& phy = input.AddComponent<cPhysics_Kinematic>();
		phy.localVelocity = false;
		phy.ph_maxSpeed = 5.0f;
		phy.ph_maxAcceleration = 10.0f;
	}

	if (!input.TryGetComponent<CombatComponent>())
	{
		input.AddComponent<CombatComponent>();
	}
	return true;
}

