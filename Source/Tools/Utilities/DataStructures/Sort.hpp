#pragma once	
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional> 
	template <class T>
	void SelectionSort(std::vector<T>& aVector)
	{
		for(int i = 0; i < aVector.size() - 1; i++)
		{
			int index = i;
			for(int j = i + 1; j < aVector.size(); j++)
			{
				if(aVector[j] < aVector[index])
				{
					index = j;
				}
			}
			if(index != i)
			{
				std::swap(aVector[i],aVector[index]);
			}
		}
	}

	template <class T>
	void BubbleSort(std::vector<T>& aVector)
	{
		int length = static_cast<int>(aVector.size());
		while(length > 1)
		{
			for(size_t i = 0; i < length - 1; i++)
			{
				if(aVector[i + 1] < aVector[i])
				{
					std::swap(aVector[i],aVector[i + 1]);
				}
			}
			length--;
		}
	}

	template <class T>
	void QuickSort(std::vector<T>& aVector,int low,int high)
	{
		if(low >= 0 && high >= 0 && low < high)
		{
			int pivot = CommonUtilities::internal_partition<T>(aVector,low,high);

			CommonUtilities::QuickSort<T>(aVector,low,pivot);  //Could be done sending iterators instead
			CommonUtilities::QuickSort<T>(aVector,pivot + 1,high); 
		}
	}

	template <class T>
	int internal_partition(std::vector<T>& aVector,int low,int high)
	{
		//Find a pivot in the middle of the array
		T pivot = aVector[static_cast<int>(floor((high - low) / 2.0f)) + low];
		// find a left index and a right index
		int leftIndex = low - 1;
		int rightIndex = high + 1;
		// move toward pivot unill they cross each other
		int max = 0;
		while(max < INT_MAX) //   nasa thingy   max limit
		{
			max++;

			leftIndex++;
			while(aVector[leftIndex] < pivot)
			{
				leftIndex++;
			}

			rightIndex--;
			while(pivot < aVector[rightIndex])
			{
				rightIndex--;
			}

			if(leftIndex >= rightIndex)
			{
				return rightIndex;
			}

			std::swap(aVector[leftIndex],aVector[rightIndex]); 
		}
		return 0;
	}

	template <class T>
	void QuickSort(std::vector<T>& aVector)
	{
		int pivot = CommonUtilities::internal_partition<T>(aVector,0,static_cast<int>(aVector.size()) - 1);

		CommonUtilities::QuickSort<T>(aVector,0,pivot); // Start a new sort on the left partition, will run once in this overloaded function
		CommonUtilities::QuickSort<T>(aVector,pivot + 1,static_cast<int>(aVector.size()) - 1); // Start a new sort on the right partition, will run once in this overloaded function
	}


	template <class T>
	void internal_merge(std::vector<T>& aVector,int leftStart,int midpoint, int rightEnd)
	{   
		int j = leftStart,k = midpoint+1; 
		std::vector<T> merged;
		merged.reserve(rightEnd - leftStart + 1);

		while(j <= midpoint  && k <= rightEnd)
		{
			if(aVector[j] < aVector[k]) // if left < then put in i slot of return array, move start pos of left one element
			{
				merged.push_back(aVector[j]);
				j++;
			}
			else
			{
				merged.push_back(aVector[k]);
				k++;
			} 
		}

		while(j <= midpoint) 
		{
			merged.push_back(aVector[j]);
			j++; 
		}
		while(k <= rightEnd)
		{
			merged.push_back(aVector[k]);
			k++;  
		} 

		for(size_t i = leftStart; i <= rightEnd; i++)
		{
			aVector[i] = merged[i - leftStart];
		}
	}

	template <class T>
	void MergeSort(std::vector<T>& aVector,int leftStart,int rightEnd)
	{ 
		if(leftStart >= rightEnd)
		{
			return;
		} 

		int mid = leftStart + (rightEnd - leftStart) / 2; 

		MergeSort(aVector,leftStart,mid);
		MergeSort(aVector,mid + 1,rightEnd);

		internal_merge(aVector,leftStart,mid,rightEnd);		
	}

	template <class T>
	void MergeSort(std::vector<T>& aVector)
	{		 
		MergeSort(aVector,0,static_cast<int>(aVector.size() - 1));
	} 
