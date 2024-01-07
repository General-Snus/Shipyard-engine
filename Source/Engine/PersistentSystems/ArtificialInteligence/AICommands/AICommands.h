#pragma once

class GameObject;
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

	//Action functions
	bool ShootAtTarget(GameObject input);
	bool Retreat(GameObject input);
	bool MoveFreely(GameObject input);
	bool MoveToward(GameObject input);
	bool AlignToTarget(GameObject input);
	bool DeathSpin(GameObject input);
	bool IsFullyHealed(GameObject input);
}

