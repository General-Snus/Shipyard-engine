#include <ShipyardEngine.pch.h>

#include "../Decicion1States.h"
#include "../StateMachineController.h"

StateMachineController::StateMachineController(StateMachineBase decicionTree)
{
}

StateMachineController::StateMachineController()
{
    myMachine.AddState((int)eStates::Movement, std::make_shared<MovementState>());
    myMachine.AddState((int)eStates::Fighting, std::make_shared<FightingState>());
    myMachine.AddState((int)eStates::Fleeing, std::make_shared<FleeingState>());
    myMachine.AddState((int)eStates::Healing, std::make_shared<HealingState>());
    myMachine.AddState((int)eStates::Dead, std::make_shared<DeadState>());

    myMachine.ChangeState((int)eStates::Movement);
}

bool StateMachineController::Update(GameObject input)
{
    myMachine.Update(input);

    auto &physicsComponent = input.GetComponent<cPhysics_Kinematic>();
    auto &transform = input.GetComponent<Transform>();

    // Corrects tree position
    Vector3f newPosition = SteeringBehaviour::SetPositionInBounds(transform.GetPosition(), 50.0f);
    transform.SetPosition(newPosition);

    Vector3f normal;
    Vector3f ref = AIPollingManagerInstance
                       .GetStation<MultipleTargets_PollingStation>("Colliders")
                       ->GetClosestPositionOnCollider(transform.GetPosition(), normal);
    DebugDrawer::Get().AddDebugLine(transform.GetPosition(), ref, Vector3f(0.0f, 1.0f, 0.01f), 0.01f);
    SteeringBehaviour::Separation(ref, &physicsComponent, transform.GetPosition());

    return false;
}

bool StateMachineController::ComponentRequirement(GameObject input)
{
    if (!input.TryGetComponent<cPhysics_Kinematic>())
    {
        auto &phy = input.AddComponent<cPhysics_Kinematic>();
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
