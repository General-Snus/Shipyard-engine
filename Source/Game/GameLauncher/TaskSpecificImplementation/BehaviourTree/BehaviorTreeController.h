#pragma once
#include <Engine/AssetManager/Objects/AI/AgentSystem/BehaviourTree/BrainTree.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h>

class BehaviorTreeController : public Controller
{
  public:
    MYLIB_REFLECTABLE()
    explicit BehaviorTreeController(BrainTree::BehaviorTree tree);
    bool Update(GameObject input) override;

    bool ComponentRequirement(GameObject input) override;
    bool InspectorView() override;
  private:
    BrainTree::BehaviorTree m_Tree;
};

REFL_AUTO(type(BehaviorTreeController))
