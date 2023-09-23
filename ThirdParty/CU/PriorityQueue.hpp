#pragma once
#include <vector>
#include <assert.h>
#include "Heap.hpp" 

namespace CommonUtilities {
	template <class T,class P>
	struct PriorityQueueElement
	{
	public:
		T value;
		P priority;

		PriorityQueueElement(const T& val,P pri) : value(val),priority(pri) {}
		PriorityQueueElement() : value(0),priority(0) {} 
	};
	template <class T,class P>
	inline bool operator< (const PriorityQueueElement<T,P>& lhs,const PriorityQueueElement<T,P>& rhs)
	{
		return lhs.priority < rhs.priority;
	}

	template <class T,class P>
	class PriorityQueue : public Heap<PriorityQueueElement<T,P>>
	{
	public: 
		using Heap<PriorityQueueElement<T,P>>::Heap;
		//Lägger till elementet i heapen
		void Enqueue(const T& value,P priority); //lägger till elementet i heapen
		//Returnerar det största elementet i heapen
		const PriorityQueueElement<T,P>& GetTop() const;
		//Tar bort det största elementet ur heapen och returnerar det
		T Dequeue(); 
		bool Contains(const T& value);
		void ChangePriority(const T& value,P newPriority);

	}; 
	template <class T,class P>
	inline void PriorityQueue<T,P>::Enqueue(const T& value,P priority)
	{
		PriorityQueueElement<T,P> element(value,priority);
		Heap<PriorityQueueElement<T,P>>::Enqueue(element);
	}
	template <class T,class P>
	inline const PriorityQueueElement<T,P>& PriorityQueue<T,P>::GetTop() const
	{
		return Heap<PriorityQueueElement<T>>::GetTop();
	}

	

	template <class T,class P>
	inline T PriorityQueue<T,P>::Dequeue()
	{
		return Heap<PriorityQueueElement<T,P>>::Dequeue().value;
	}
	template <class T,class P>
	inline bool PriorityQueue<T,P>::Contains(const T& value)
	{
		for(int i = 0; i < GetSize(); i++)
		{
			if(GetArray()[i].value == value)
			{
				return true;
			}
		}
		return false;
	}

	template <class T,class P>
	inline void PriorityQueue<T,P>::ChangePriority(const T& value,P newPriority)
	{
		for(int i = 0; i < GetSize(); i++)
		{
			if(GetArray()[i].value == value)
			{
				GetArray()[i].priority = newPriority;
				Upshift(i); // Reorder the element in the heap
				return;
			}
		}
	}
}