#pragma once
 
	template<class T>
	class Node;

	template <class T>
	class BSTSet
	{
	public:
		BSTSet();
		~BSTSet();

		//Returnerar true om elementet finns i mängden, och false annars.
		bool HasElement(const T& aValue);
		//Stoppar in elementet i mängden, om det inte redan finns där. Gör ingenting annars.
		void Insert(const T& aValue);
		//Plockar bort elementet ur mängden, om det finns. Gör ingenting annars.
		void Remove(const T& aValue);

		int Compare(const T& lhs,const T& rhs) const;

	private:
		Node<T>* Get(Node<T>* aNode,const T& aValue);
		Node<T>* Put(Node<T>* aNode,const T& aValue);

		Node<T>* Min(Node<T>* aNode);
		Node<T>* DeleteMin(Node<T>* aNode);
		Node<T>* Delete(Node<T>* aNode,const T& aValue);

		int Size(Node<T>* aNode);
		Node<T>* myRoot;
	};

	template <class T>
	class Node
	{
	public:
		Node(T val,int N);
		~Node();

		T myValue;
		Node* myLeftChild;
		Node* myRightChild;
		int myN;
	};

	template<class T>
	inline BSTSet<T>::BSTSet()
	{
		myRoot = nullptr;
	}

	template<class T>
	inline BSTSet<T>::~BSTSet()
	{
		delete myRoot;
		myRoot = nullptr;
	}

	template<class T>
	inline bool BSTSet<T>::HasElement(const T& aValue)
	{
		if(Get(myRoot,aValue) != nullptr)
		{
			return true;
		}

		return false;
	}
	template<class T>
	inline void BSTSet<T>::Insert(const T& aValue)
	{
		myRoot = Put(myRoot,aValue);
	}
	template<class T>
	inline void BSTSet<T>::Remove(const T& aValue)
	{
		myRoot = Delete(myRoot,aValue);
	}
	template<class T>
	int BSTSet<T>::Compare(const T& lhs,const T& rhs) const
	{
		if((!(lhs < rhs) && !(rhs < lhs)))
		{
			return 0;
		}

		else if(lhs < rhs)
		{
			return -1;
		}

		else
		{
			return 1;
		}
	}

	template<class T>
	inline Node<T>* BSTSet<T>::Get(Node<T>* aNode,const T& aValue)
	{
		if(aNode == nullptr)
		{
			return nullptr;
		}
		int comp = Compare(aValue,aNode->myValue);
		if(!comp)
		{
			return aNode;
		}
		else if(comp < 0)
		{
			return Get(aNode->myLeftChild,aValue);
		}
		else
		{
			return Get(aNode->myRightChild,aValue);
		}
	}
	template<class T>
	inline Node<T>* BSTSet<T>::Put(Node<T>* aNode,const T& aValue)
	{
		if(aNode == nullptr)
		{
			myRoot = new Node<T>(aValue,1);
			return myRoot;
		}

		int comp = Compare(aValue,aNode->myValue);

		if(!comp)
		{
			aNode->myValue = aValue;
		}
		else if(comp < 0)
		{
			aNode->myLeftChild = Put(aNode->myLeftChild,aValue);
		}
		else
		{
			aNode->myRightChild = Put(aNode->myRightChild,aValue);
		}
		aNode->myN = Size(aNode->myLeftChild) + Size(aNode->myRightChild) + 1;
		return aNode;
	}
	template<class T>
	inline Node<T>* BSTSet<T>::DeleteMin(Node<T>* aNode)
	{
		if(aNode->myLeftChild == nullptr)
		{
			return aNode->myRightChild;
		}

		aNode->myLeftChild = DeleteMin(aNode->myLeftChild);
		aNode->myN = Size(aNode->myLeftChild) + Size(aNode->myRightChild) + 1;
		return aNode;
	}
	template<class T>
	inline Node<T>* BSTSet<T>::Delete(Node<T>* aNode,const T& aValue)
	{
		if(aNode == nullptr)
		{
			return nullptr;
		}
		int comp = Compare(aValue,aNode->myValue);
		if(!comp)
		{
			if(aNode->myRightChild == nullptr)
			{
				return aNode->myLeftChild;
			}
			if(aNode->myLeftChild == nullptr)
			{
				return aNode->myRightChild;
			}
			Node<T>* t = aNode;
			aNode = Min(t->myRightChild);
			aNode->myRightChild = DeleteMin(t->myRightChild);
			aNode->myLeftChild = t->myLeftChild;
		}
		else if(comp < 0)
		{
			aNode->myLeftChild = Delete(aNode->myLeftChild,aValue);
		}
		else
		{
			aNode->myRightChild = Delete(aNode->myRightChild,aValue);
		}

		aNode->myN = Size(aNode->myLeftChild) + Size(aNode->myRightChild) + 1;
		return aNode;
	}
	template<class T>
	inline Node<T>* BSTSet<T>::Min(Node<T>* aNode)
	{
		if(aNode->myLeftChild == nullptr)
		{
			return aNode;
		}
		return Min(aNode->myLeftChild);
	}

	template<class T>
	inline int BSTSet<T>::Size(Node<T>* aNode)
	{
		if(aNode == nullptr)
		{
			return 0;
		}
		return aNode->myN;
	}

	template<class T>
	inline Node<T>::Node(T aValue,int aN)
	{
		myLeftChild = nullptr;
		myRightChild = nullptr;

		myValue = aValue;
		myN = aN;
	}
	template<class T>
	inline Node<T>::~Node()
	{
		delete myLeftChild;
		delete myRightChild;
		myLeftChild = nullptr;
		myRightChild = nullptr; 
	} 