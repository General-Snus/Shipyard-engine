#pragma once
#include <vector>
#include <assert.h>

inline int Parent(int i) { return (i - 1) / 2; }
inline int Child(int i,bool Right) { return (2 * i + (int)Right + 1); }
 
	template <class T>
	class Heap
	{  
	public:
		Heap();
		Heap(int allocate); 
		~Heap();
		//Returnerar antal element i heapen
		int GetSize() const;
		//Lägger till elementet i heapen
		void Enqueue(const T& aElement); //lägger till elementet i heapen
		//Returnerar det största elementet i heapen
		const T& GetTop() const;
		//Tar bort det största elementet ur heapen och returnerar det
		T Dequeue();

	protected:
		void Upshift(int i);
		void Downshift(int i); 
		T* GetArray();
	private:
		T* arr;
		int size;
	};
	template<class T>
	inline Heap<T>::Heap() : size(-1)
	{
		arr = new T[1000];
	}
	template<class T>
	inline Heap<T>::Heap(int allocate) : size(-1)
	{
		arr = new T[allocate];
	}
	template<class T>
	inline Heap<T>::~Heap()
	{
		delete[] arr;
	}
	template<class T>
	inline int Heap<T>::GetSize() const
	{
		return size+1;
	}
	template<class T>
	inline void Heap<T>::Enqueue(const T& aElement)
	{
		size++;
		arr[size] = aElement;
		Upshift(size);
	}
	template<class T>
	inline const T& Heap<T>::GetTop() const
	{
		assert(size > -1 && "Heap is empty");
		return arr[0];
	}

	template<class T>
	inline T Heap<T>::Dequeue()
	{
		assert(size > -1 && "Heap is empty");
		T temp = arr[0];
		arr[0] = arr[size];
		arr[size] = T();
		size--;

		Downshift(0);
		return temp;
	}

	template<class T>
	inline T* Heap<T>::GetArray()
	{
		return arr;
	}

	template<class T>
	inline void Heap<T>::Upshift(int i)
	{
		while(i > 0 && arr[Parent(i)] < arr[i])
		{
			T temp = arr[i];
			arr[i] = arr[Parent(i)];
			arr[Parent(i)] = temp;
			i = Parent(i);
		} 
	}

	template<class T>
	inline void Heap<T>::Downshift(int i)
	{

		int maxValue = i;

		int left = Child(i,false);
		int right = Child(i,true);

		if(left < size+1 && arr[maxValue] < arr[left])
		{
			maxValue = left;
		}

		if(right < size + 1 && arr[maxValue]  < arr[right])
		{
			maxValue = right;
		}

		if(i != maxValue)
		{
			T swap = arr[i];
			arr[i] = arr[maxValue];
			arr[maxValue] = swap;
			Downshift(maxValue);
		}
	}  