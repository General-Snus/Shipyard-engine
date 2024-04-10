#pragma once
#include <Engine/AssetManager/Objects/AI/AgentSystem/Controllers/Controller.h>
#include <Engine/AssetManager/Objects/AI/AgentSystem/DecisionTree/DecisionTree.h>

class DecisionTreeController : public Controller
{
public:
	explicit DecisionTreeController(const DecisionTree& decicionTree);
	explicit DecisionTreeController();
	bool Update(GameObject input) override;


	bool ComponentRequirement(GameObject input) override;
private:
	DecisionTree myTree;
private:
	//Decicion functions 
};

