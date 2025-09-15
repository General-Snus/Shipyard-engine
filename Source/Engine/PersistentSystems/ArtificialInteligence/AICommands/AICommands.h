#pragma once
#include <Engine/AssetManager/ComponentSystem/GameObject.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/BehaviourTree/BrainTree.h>
#include <Engine/AssetManager/Reflection/Reflectable.h>

/// <summary>
/// Thought process was something like this:
/// if i have to duplicate commands for each agent i could use a general way of locating and structuring the commands
/// while having to get the gameobject and getting each component for every command is bad,
/// it also makes them completely generic and plug and play
///
/// now how can i force this structure on new development/developers? pondering the orb..
/// </summary>
namespace GeneralizedAICommands
{
bool IsTargetInSight(GameObject input);
bool IsTargetInRange(GameObject input);
bool IsTargetAlive(GameObject input);
bool IsDead(GameObject input);
bool IsHealthy(GameObject input);
bool IsFighting(GameObject input);
bool IsWinning(GameObject input);

// Action functions
bool ShootAtTarget(GameObject input);
bool Retreat(GameObject input);
bool MoveFreely(GameObject input);
bool MoveToward(GameObject input);
bool AlignToTarget(GameObject input);
bool DeathSpin(GameObject input);
bool IsFullyHealed(GameObject input);
} // namespace GeneralizedAICommands

namespace BehaviourTreeAICommands
{
class IsTargetInSight : public Reflectable<IsTargetInSight>, public BrainTree::Leaf
{
  public:
    reflectable(IsTargetInSight);
    using BrainTree::Leaf::Leaf;

    Status update() override;

  private:
};

class IsTargetInRange : public Reflectable<IsTargetInRange>, public BrainTree::Leaf
{
  public:
    reflectable(IsTargetInRange);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};

class IsTargetAlive : public Reflectable<IsTargetAlive>, public BrainTree::Leaf
{
  public:
    reflectable(IsTargetAlive);
    using BrainTree::Leaf::Leaf;

    Status update() override;

  private:
};

class IsDead : public Reflectable<IsDead>, public BrainTree::Decorator
{
  public:
    reflectable(IsDead);
    Status update() override;

  private:
};

class IsHealthy : public Reflectable<IsHealthy>, public BrainTree::Decorator
{
  public:
    reflectable(IsHealthy);

    Status update() override;

  private:
};

// Action functions
class ShootAtTarget : public Reflectable<ShootAtTarget>, public BrainTree::Leaf
{
  public:
    reflectable(ShootAtTarget);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};
class Retreat : public Reflectable<Retreat>, public BrainTree::Leaf
{
  public:
    reflectable(Retreat);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};
class MoveFreely : public Reflectable<MoveFreely>, public BrainTree::Leaf
{
  public:
    reflectable(MoveFreely);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};
class AlignToTarget : public Reflectable<AlignToTarget>, public BrainTree::Leaf
{
  public:
    reflectable(AlignToTarget);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};
class DeathSpin : public Reflectable<DeathSpin>, public BrainTree::Leaf
{
  public:
    reflectable(DeathSpin);
    using BrainTree::Leaf::Leaf;

    Status update() override;
};
class IsFullyHealed : public Reflectable<IsFullyHealed>, public BrainTree::Decorator
{
  public:
    reflectable(IsFullyHealed);
    Status update() override;
};

} // namespace BehaviourTreeAICommands

REFL_AUTO(type(BehaviourTreeAICommands::IsTargetInSight))
REFL_AUTO(type(BehaviourTreeAICommands::IsTargetInRange))
REFL_AUTO(type(BehaviourTreeAICommands::IsTargetAlive))
REFL_AUTO(type(BehaviourTreeAICommands::IsDead))
REFL_AUTO(type(BehaviourTreeAICommands::IsHealthy))
REFL_AUTO(type(BehaviourTreeAICommands::ShootAtTarget))
REFL_AUTO(type(BehaviourTreeAICommands::Retreat))
REFL_AUTO(type(BehaviourTreeAICommands::MoveFreely))
REFL_AUTO(type(BehaviourTreeAICommands::AlignToTarget))
REFL_AUTO(type(BehaviourTreeAICommands::DeathSpin))
REFL_AUTO(type(BehaviourTreeAICommands::IsFullyHealed))
