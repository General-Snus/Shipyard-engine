#include "../BehaviorTreeController.h"
#include <Engine/AssetManager/ComponentSystem/Components/ActorSystem/CombatComponent.h>
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Engine/AssetManager/ComponentSystem/Components/Transform.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/AIPollingManager.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/SteeringBehaviour.h>

BehaviorTreeController::BehaviorTreeController(BrainTree::BehaviorTree tree) : m_Tree(tree)

{
}

bool BehaviorTreeController::Update(GameObject input)
{
    m_Tree.getBlackboard()->setGameObject("interatingGameobject", input);

    auto &physicsComponent = input.GetComponent<cPhysics_Kinematic>();
    auto &transform = input.GetComponent<Transform>();

    // Corrects tree position
    Vector3f newPosition = SteeringBehaviour::SetPositionInBounds(transform.GetPosition(), 50.0f);
    transform.SetPosition(newPosition);

    Vector3f normal;
    Vector3f ref = AIPollingManager::Get()
                       .GetStation<MultipleTargets_PollingStation>("Colliders")
                       ->GetClosestPositionOnCollider(transform.GetPosition(), normal);
    DebugDrawer::Get().AddDebugLine(transform.GetPosition(), ref, Vector3f(0.0f, 1.0f, 0.01f), 0.01f);
    SteeringBehaviour::Separation(ref, &physicsComponent, transform.GetPosition());

    switch (m_Tree.update())
    {
    case BrainTree::Node::Status::Success:
        return true;
    case BrainTree::Node::Status::Failure:
        return false;
    case BrainTree::Node::Status::Running:
        return true;
    default:
        return false;
    };
}

bool BehaviorTreeController::ComponentRequirement(GameObject input)
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

bool BehaviorTreeController::InspectorView()
{
    for (auto &[node, status] : m_Tree.getBlackboard()->getAllStatus())
    {
        std::string name;
        if (node->getName().empty())
        {

            name = std::format("{}\n\t- {}", node->GetTypeInfo().Name(),
                               std::string(magic_enum::enum_name(node->getStatus())));
        }
        else
        {
            name = std::format("{}\n\t- {}\n\t- {}", node->GetTypeInfo().Name(), node->getName(),
                               std::string(magic_enum::enum_name(node->getStatus())));
        }
        ImGui::Text(name.c_str());
    }

    return false;
}
