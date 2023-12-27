#pragma once
#include "../DecisionTree/DecisionTree.h"
#include "Controller.h"

class DecisionTreeController : public Controller
{
public:
	explicit DecisionTreeController(DecisionTree decicionTree);
	explicit DecisionTreeController();
	bool Update(GameObject input) override;


	bool ComponentRequirement(GameObject input) override;
private:
	DecisionTree myTree;
private:
	//Decicion functions 
};

