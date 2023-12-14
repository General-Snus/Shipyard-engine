#include "AssetManager.pch.h"
#include "../DecicionTree.h"

bool DecicionTree::Node::Evaluate()
{
    return false;
}

DecicionTree::DecicionTree()
{
   // myNodes.resize(1000);
}

DecicionTree::~DecicionTree()
{
}

void DecicionTree::AddNodeAt(int at,std::function<bool> evaluatingFunction) 
{
    if(at >= myNodes.size())
    {
		//myNodes.resize(at*2); // It was at this moment he knew, he fucked up        this is ass
	}

    myNodes[at] = Node(evaluatingFunction);
}

void DecicionTree::AddChildNodeAt(int at,std::function<bool> evaluatingFunction)
{
}

bool DecicionTree::RunTree()
{
    if(myNodes.empty())
    {
        return false;
    }

    //Limit to max possible iterations
    int TargetNode = 0;
    for(size_t i = 0; i < myNodes.size(); i++)
    {
        if(myNodes.at(TargetNode).Evaluate())
        {
            TargetNode = this->GetChildOf(TargetNode)[0]; 
        }
		else
		{
            TargetNode = this->GetChildOf(TargetNode)[1];
        }

        if(TargetNode == -1)
        {
            break;
        }
    }



    return true;
}

int DecicionTree::GetTreeSize()
{
    return myNodes.size();
}

int DecicionTree::GetNumberOfLogicNodes()
{
    return 0;
}

int DecicionTree::GetNumberOfDecicion()
{
    return 0;
}

int DecicionTree::RunRecusively(int target)
{
    return 0;
}

void DecicionTree::ReorderTree()
{
}

std::array<int,2> DecicionTree::GetChildOf(int node)
{
    std::array<int,2> returnArray = {-1,-1};
    if(myNodes.contains(node * 2))
    {
        returnArray[0] = node * 2;
    }

    if(myNodes.contains(node * 2 + 1))
    {
        returnArray[1] = node * 2 + 1;
    }

    return returnArray;
}
