#pragma once

namespace CommonUtilities
{
	template<class T>
	class DoublyLinkedListNode;

	template <class T>
	class DoublyLinkedList
	{
	public:
		DoublyLinkedList();
		~DoublyLinkedList();

		int GetSize() const;

		DoublyLinkedListNode<T>* GetFirst();
		DoublyLinkedListNode<T>* GetLast();

		void InsertFirst(const T& aValue);
		void InsertLast(const T& aValue);
		void InsertBefore(DoublyLinkedListNode<T>* aNode,const T& aValue);
		void InsertAfter(DoublyLinkedListNode<T>* aNode,const T& aValue);
		void Remove(DoublyLinkedListNode<T>* aNode);

		DoublyLinkedListNode<T>* FindFirst(const T& aValue);
		DoublyLinkedListNode<T>* FindLast(const T& aValue);

		bool RemoveFirst(const T& aValue);
		bool RemoveLast(const T& aValue);

	private:
		DoublyLinkedListNode<T>* myFirst;
		DoublyLinkedListNode<T>* myLast;
		int mySize;
	};

	template <class T>
	class DoublyLinkedListNode
	{
	public:
		DoublyLinkedListNode<T>(const DoublyLinkedListNode<T>&) = delete;
		DoublyLinkedListNode<T>& operator=(const DoublyLinkedListNode<T>&) = delete;

		const T& GetValue() const;
		T& GetValue();

		DoublyLinkedListNode<T>* GetNext() const;
		DoublyLinkedListNode<T>* GetPrevious() const;
	private:
		friend class DoublyLinkedList<T>;
		DoublyLinkedListNode(const T& aValue);
		~DoublyLinkedListNode()
		{
		}

		T myData;
		DoublyLinkedListNode* myPrevious;
		DoublyLinkedListNode* myNext;
	};

	template<class T>
	DoublyLinkedListNode<T>::DoublyLinkedListNode(const T& aValue)
	{
		myData = aValue;
		myPrevious = nullptr;
		myNext = nullptr;
	}

	//Nodes
	template<class T>
	const T& DoublyLinkedListNode<T>::GetValue() const
	{
		return myData;
	}

	template<class T>
	T& DoublyLinkedListNode<T>::GetValue()
	{
		return myData;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetNext() const
	{
		return myNext;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedListNode<T>::GetPrevious() const
	{
		return myPrevious;
	}

	//List
	template<class T>
	DoublyLinkedList<T>::DoublyLinkedList()
	{
		mySize = 0;
		myFirst = nullptr;
		myLast = nullptr;
	}

	template<class T>
	DoublyLinkedList<T>::~DoublyLinkedList()
	{
		if(myLast != nullptr)
		{
			DoublyLinkedListNode<T>* iterator = myLast;
			while(iterator->myPrevious != nullptr)
			{
				iterator = iterator->myPrevious;
				delete iterator->myNext;
			}
			delete iterator;
		}
	}

	template<class T>
	int DoublyLinkedList<T>::GetSize() const
	{
		return mySize;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetFirst()
	{
		return myFirst;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::GetLast()
	{
		return myLast;
	}

	template<class T>
	void DoublyLinkedList<T>::InsertFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);
		if(myFirst != nullptr)
		{
			newNode->myNext = myFirst;
			myFirst->myPrevious = newNode;
			myFirst = newNode;
		}
		else
		{
			myFirst = newNode;
			myLast = newNode;
		}
		mySize++;
	}

	template<class T>
	void DoublyLinkedList<T>::InsertLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);
		if(mySize != 0)
		{
			newNode->myPrevious = myLast;
			myLast->myNext = newNode;
			myLast = newNode;
		}
		else
		{
			myFirst = newNode;
			myLast = newNode;
		}
		mySize++;
	}

	template<class T>
	void DoublyLinkedList<T>::InsertBefore(DoublyLinkedListNode<T>* aNode,const T& aValue)
	{
		if(aNode != nullptr)
		{
			DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);

			newNode->myNext = aNode;
			newNode->myPrevious = aNode->myPrevious;
			aNode->myPrevious = newNode;

			if(newNode->myPrevious != nullptr)
			{
				newNode->myPrevious->myNext = newNode;
			}
			else
			{
				myFirst = newNode;
			}
			mySize++;
			return;
		}
	}

	template<class T>
	void DoublyLinkedList<T>::InsertAfter(DoublyLinkedListNode<T>* aNode,const T& aValue)
	{
		if(aNode != nullptr)
		{
			DoublyLinkedListNode<T>* newNode = new DoublyLinkedListNode<T>(aValue);
			if(aNode->myNext != nullptr)
			{
				newNode->myNext = aNode->myNext;
				newNode->myNext->myPrevious = newNode;
			}
			else
			{
				myLast = newNode;
			}
			newNode->myPrevious = aNode;
			aNode->myNext = newNode;
			mySize++;
			return;
		}
	}

	template<class T>
	void DoublyLinkedList<T>::Remove(DoublyLinkedListNode<T>* aNode)
	{
		if(aNode != nullptr)
		{
			if(aNode->myPrevious != nullptr)
			{
				aNode->myPrevious->myNext = aNode->myNext;
			}
			else
			{
				myFirst = aNode->myNext;
			}

			if(aNode->myNext != nullptr)
			{
				aNode->myNext->myPrevious = aNode->myPrevious;
			}
			else
			{
				myLast = aNode->myPrevious;
			}

			delete aNode;
			mySize--;
		}
		return;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* iterator = myFirst;
		while(iterator != nullptr)
		{
			if(iterator->myData == aValue)
			{
				return iterator;
			}
			iterator = iterator->myNext;
		}
		return nullptr;
	}

	template<class T>
	DoublyLinkedListNode<T>* DoublyLinkedList<T>::FindLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* iterator = myLast;
		while(iterator != nullptr)
		{
			if(iterator->myData == aValue)
			{
				return iterator;
			}
			iterator = iterator->myPrevious;
		}
		return nullptr;
	}

	template<class T>
	bool DoublyLinkedList<T>::RemoveFirst(const T& aValue)
	{
		DoublyLinkedListNode<T>* iterator = myFirst;
		while(iterator != nullptr)
		{
			if(iterator->myData == aValue)
			{
				if(iterator->myPrevious != nullptr)
				{
					iterator->myPrevious->myNext = iterator->myNext;
				}
				else
				{
					myFirst = iterator->myNext;
				}
				if(iterator->myNext != nullptr)
				{
					iterator->myNext->myPrevious = iterator->myPrevious;
				}
				else
				{
					myLast = iterator->myPrevious;
				}

				delete iterator;
				mySize--;

				return true;
			}
			iterator = iterator->myNext;
		}
		return false;
	}

	template<class T>
	bool DoublyLinkedList<T>::RemoveLast(const T& aValue)
	{
		DoublyLinkedListNode<T>* iterator = myLast;
		while(iterator != nullptr)
		{
			if(iterator->myData == aValue)
			{
				if(iterator->myPrevious != nullptr)
				{
					iterator->myPrevious->myNext = iterator->myNext;
				}
				else
				{
					myFirst = iterator->myNext;
				}
				if(iterator->myNext != nullptr)
				{
					iterator->myNext->myPrevious = iterator->myPrevious;
				}
				else
				{
					myLast = iterator->myPrevious;
				} 

				delete iterator;
				mySize--;

				return true;
			}
			iterator = iterator->myPrevious;
		}
		return false;
	}
}