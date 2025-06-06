#pragma once
#include <vector>
#include <unordered_map>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

constexpr float nodeDistance = 1.0f;
class Actor;
class Controller;

enum class eAIEvent
{
	playerHacking, stop ,count
};
struct AIEvent
{
	Vector3f position;
	eAIEvent type;
};


class AIEventManager
{
public:  
	void RegisterListener(const eAIEvent eventType,Controller* aController);
	void Update(); 
	static AIEventManager& Instance()
	{
		static AIEventManager instance;
		return instance;
	};
private: 
	AIEventManager(); 
	void DispatchEvent(const AIEvent& aEvent); 
	std::unordered_map<eAIEvent, std::vector<Controller*>> eventListener;  
};
 