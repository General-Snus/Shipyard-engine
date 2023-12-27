#pragma once
#include <functional>


class DecisionTree
{
private:
	friend class Node;

	class Node
	{
	public:
		explicit Node(std::function<bool(GameObject)> func);
		std::function<bool(GameObject)> myCondition;
		bool Evaluate(GameObject inp) const;
	private:
		bool isLeaf;
		int nodePosition;
		std::array<int,2> myChildNodes;
	};

public:
	DecisionTree();
	~DecisionTree();

	void AddNodeAt(int at,std::function<bool(GameObject)>);
	template<typename F>
	void AddChildNodeAt(int at,bool atPosetiveAnswer,F&& f);


	bool RunTree(GameObject input);
	bool RunTree();
	int GetTreeSize();
	int GetNumberOfLogicNodes();
	int GetNumberOfDecicion();
private:
	GameObject myContextObject;

	int RunRecusively(int target);
	void ReorderTree();
	//-1 is no child
	std::array<int,2> GetChildOf(int node);
	//Follow the binomial tree and rearage after each add.
	std::unordered_map<int,Node> myNodes;

};

