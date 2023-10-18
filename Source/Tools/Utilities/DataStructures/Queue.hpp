#pragma once
#include <vector>
#include <assert.h>

namespace CommonUtilities {


	template <class T>
	class Queue
	{
	public:
		//Skapar en tom k�
		Queue();
		~Queue();
		//Returnerar antal element i k�n
		int GetSize() const;
		//Returnerar elementet l�ngst fram i k�n. Kraschar med en assert om k�n �r
		//tom
		const T& GetFront() const;
		//Returnerar elementet l�ngst fram i k�n. Kraschar med en assert om k�n �r tom
		T& GetFront();
		//L�gger in ett nytt element l�ngst bak i k�n
		void Enqueue(const T& aValue);
		void EnqueueUnique(const T& aValue);
		//Tar bort elementet l�ngst fram i k�n och returnerar det. Kraschar med en
		//assert om k�n �r tom.
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