#include "AssetManager.pch.h"

#include "../DecisionTree.h" 
DecisionTree::Node::Node(const std::function<bool(GameObject)>& func) : myCondition(func)
{
}

bool DecisionTree::Node::Evaluate(GameObject inp) const
{
	return myCondition(inp);
}

DecisionTree::DecisionTree()
{
	// myNodes.resize(1000);
}

DecisionTree::~DecisionTree()
{
}
//template<typename f> 
//inline void DecicionTree::AddNodeAt(int at,bool(f::* func))
//{
//	if(at >= myNodes.size())
//	{
//		//myNodes.resize(at*2); // It was at this moment he knew, he fucked up        this is ass
//	}
//
//	myNodes.try_emplace(at,Node(func));
//}

void DecisionTree::AddNodeAt(int at,const std::function<bool(GameObject)>& func)
{
	if (at >= myNodes.size())
	{
		//myNodes.resize(at*2); // It was at this moment he knew, he fucked up        this is ass
	}

	myNodes.try_emplace(at,Node(func));
}

int DecisionTree::AddChildNodeAt(int at,bool atPositiveAnswer,const std::function<bool(GameObject)>& func)
{
	int TargetNode = -1;
	if (atPositiveAnswer)
	{
		TargetNode = (at * 2) + 1;
	}
	else
	{
		TargetNode = (at + 1) * 2;
	}
	myNodes.try_emplace(TargetNode,Node(func));
	return TargetNode;
}

bool DecisionTree::RunTree()
{
	return RunTree(GameObject());
}
bool DecisionTree::RunTree(GameObject input)
{
	myContextObject = input;


	if (myNodes.empty())
	{
		return false;
	}

	//Limit to max possible iterations
	int TargetNode = 0;
	for (size_t i = 0; i < myNodes.size(); i++)
	{
		if (myNodes.at(TargetNode).Evaluate(input))
		{
			TargetNode = this->GetChildOf(TargetNode)[0];
		}
		else
		{
			TargetNode = this->GetChildOf(TargetNode)[1];
		}

		if (TargetNode == -1)
		{
			break;
		}
	}



	return true;
}

int DecisionTree::GetTreeSize()
{
	return static_cast<int>(myNodes.size());
}

int DecisionTree::GetNumberOfLogicNodes()
{
	return 0;
}

int DecisionTree::GetNumberOfDecision()
{
	return 0;
}

int DecisionTree::RunRecusively(int target)
{
	target;
	return 0;
}

void DecisionTree::ReorderTree()
{
}

std::array<int,2> DecisionTree::GetChildOf(int node)
{
	std::array<int,2> returnArray = { -1,-1 };
	if (myNodes.contains((node * 2) + 1)) // true
	{
		returnArray[0] = (node * 2) + 1;
	}

	if (myNodes.contains((node + 1) * 2)) // false
	{
		returnArray[1] = (node + 1) * 2;
	}

	return returnArray;
}
