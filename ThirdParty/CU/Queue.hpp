#pragma once
#include <vector>
#include <assert.h>

namespace CommonUtilities {


	template <class T>
	class Queue
	{
	public:
		//Skapar en tom kö
		Queue();
		~Queue();
		//Returnerar antal element i kön
		int GetSize() const;
		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är
		//tom
		const T& GetFront() const;
		//Returnerar elementet längst fram i kön. Kraschar med en assert om kön är tom
		T& GetFront();
		//Lägger in ett nytt element längst bak i kön
		void Enqueue(const T& aValue);
		void EnqueueUnique(const T& aValue);
		//Tar bort elementet längst fram i kön och returnerar det. Kraschar med en
		//assert om kön är tom.
		T Dequeue(); 
	private:
		T* myContainer;
		int myFront; 

		int mySize;
		int myAllocated;
	};
	template<class T>
	inline Queue<T>::Queue()
	{
		myAllocated = 10;
		myContainer = new T[myAllocated];
		myFront = 0; 
		mySize = 0;
	}
	template<class T>
	inline Queue<T>::~Queue()
	{
		delete[] myContainer;
	}
	template<class T>
	inline int Queue<T>::GetSize() const
	{
		return mySize;
	}
	template<class T>
	inline const T& Queue<T>::GetFront() const
	{
		assert(mySize > 0 && "Queue is empty, can not get element"); 
		return myContainer[myFront];
	}
	template<class T>
	inline T& Queue<T>::GetFront()
	{
		assert(mySize > 0 && "Queue is empty, can not get element");
		return myContainer[myFront];
	}
	template<class T>
	inline void Queue<T>::EnqueueUnique(const T& aValue)
	{
		for(size_t i = 0; i < mySize; i++)
		{
			if(myContainer[i] == aValue)
			{
				return;
			}
		}
		Enqueue(aValue);
	}

	template<class T>
	inline void Queue<T>::Enqueue(const T& aValue)
	{
		if(mySize == myAllocated) // array is full, allocate more memory
		{
			T* newArray = new T[myAllocated*2];
			//std::copy(ptrmyFront,ptrBack +1,newArray); 
			for(int i = 0; i < mySize; i++)
			{
				newArray[i] = myContainer[myFront++ % myAllocated];
			}
			
			delete[] myContainer;  
			myContainer = newArray;
			myFront = 0;
			myAllocated *= 2;
		}
		myContainer[(myFront+mySize) % myAllocated] = aValue;
		mySize++;  
	}
	template<class T>
	inline T Queue<T>::Dequeue()
	{
		assert(mySize > 0 && "Queue is empty, can not dequeue");
		int returnValue = myFront;
		myFront = (myFront + 1) % myAllocated;
		mySize--;
		return myContainer[returnValue];
	}  
}