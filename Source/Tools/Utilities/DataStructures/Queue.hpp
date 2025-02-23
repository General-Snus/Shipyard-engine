#pragma once
#include <assert.h>
#include <mutex>
#include <vector>

template <class T>
class Queue
{
public: 
	Queue();
	~Queue(); 
	int GetSize() const; 
	const T& GetFront() const; 
	T& GetFront(); 
	void Enqueue(const T& aValue);
	void EnqueueUnique(const T& aValue); 
	T Dequeue();


	class Iterator {
	public:
		Iterator(T* ptr,int index,int size,int allocated)
			: ptr(ptr),index(index),size(size),allocated(allocated) {}

		Iterator& operator++() {
			index = (index + 1) % allocated;
			--size;
			return *this;
		}

		bool operator!=(const Iterator& other) const {
			return size != other.size;
		}

		T& operator*() const {
			return ptr[index];
		}

	private:
		T* ptr;
		int index;
		int size;
		int allocated;
	};

	Iterator begin() const {
		return Iterator(myContainer,myFront,mySize,myAllocated);
	}

	Iterator end() const {
		return Iterator(myContainer,(myFront + mySize) % myAllocated,0,myAllocated);
	}


private:
	T*  myContainer;
	int myFront;

	int        mySize;
	int        myAllocated;
	std::mutex dequeMutex;
};

template <class T>
Queue<T>::Queue()
{
	myAllocated = 10;
	myContainer = new T[myAllocated];
	myFront = 0;
	mySize = 0;
}

template <class T>
Queue<T>::~Queue()
{
	delete[] myContainer;
}

template <class T>
int Queue<T>::GetSize() const
{
	return mySize;
}

template <class T>
const T& Queue<T>::GetFront() const
{
	assert(mySize > 0 && "Queue is empty, can not get element");
	return myContainer[myFront];
}

template <class T>
T& Queue<T>::GetFront()
{
	assert(mySize > 0 && "Queue is empty, can not get element");
	return myContainer[myFront];
}

template <class T>
void Queue<T>::EnqueueUnique(const T& aValue)
{
	for (size_t i = 0; i < mySize; i++)
	{
		if (myContainer[i] == aValue)
		{
			return;
		}
	}
	Enqueue(aValue);
}

template <class T>
void Queue<T>::Enqueue(const T& aValue)
{
	if (mySize == myAllocated) // array is full, allocate more memory
	{
		std::scoped_lock deQueueLock(dequeMutex);

		T* newArray = new T[myAllocated * 2];
		//std::copy(ptrmyFront,ptrBack +1,newArray); 
		for (int i = 0; i < mySize; i++)
		{
			newArray[i] = myContainer[myFront++ % myAllocated];
		}

		delete[] myContainer;
		myContainer = newArray;
		myFront = 0;
		myAllocated *= 2;
	}
	myContainer[(myFront + mySize) % myAllocated] = aValue;
	mySize++;
}

template <class T>
T Queue<T>::Dequeue()
{
	std::scoped_lock deQueueLock(dequeMutex);
	assert(mySize > 0 && "Queue is empty, can not dequeue");
	int returnValue = myFront;
	myFront = (myFront + 1) % myAllocated;
	mySize--;
	return myContainer[returnValue];
}
