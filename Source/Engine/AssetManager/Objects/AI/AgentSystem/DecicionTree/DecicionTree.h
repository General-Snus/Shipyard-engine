#pragma once
#include <functional>


class DecicionTree
{
private:
	friend class Node;

	class Node
	{
	public:
		template<typename f,typename context>
		Node(bool(f::* func),context* aContext);

		std::function<bool(void)>& myCondition;
		bool Evaluate();
	private:
		bool isLeaf;
		int nodePosition;
		std::array<int,2> myChildNodes; 
	};

public:
	DecicionTree();
	~DecicionTree();
	 
	template<typename f,typename context>
	void AddNodeAt(int at,bool(f::* func),context* aContext);
	template<typename F>
	void AddChildNodeAt(int at,bool atPosetiveAnswer,F&& f);

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

