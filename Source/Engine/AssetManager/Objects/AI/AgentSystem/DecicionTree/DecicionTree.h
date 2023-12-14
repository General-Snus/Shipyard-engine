#pragma once
#include <functional>


class DecicionTree
{
private:
	friend class Node;

	class Node
	{
	public:
		Node(std::function<bool> arg);
		std::function<bool> myCondition;
		bool Evaluate();
	private:
		bool isLeaf;
		int nodePosition;
		std::array<int,2> myChildNodes;
	};

public:
	DecicionTree();
	~DecicionTree();

	void AddNodeAt(int at,std::function<bool> evaluatingFunction);
	void AddChildNodeAt(int at,std::function<bool> evaluatingFunction); 

	bool RunTree();
	int GetTreeSize();
	int GetNumberOfLogicNodes();
	int GetNumberOfDecicion();
private:
	int RunRecusively(int target);
	void ReorderTree();
	//-1 is no child
	std::array<int,2> GetChildOf(int node); 
	//Follow the binomial tree and rearage after each add.
	std::unordered_map<int,Node> myNodes;

};
