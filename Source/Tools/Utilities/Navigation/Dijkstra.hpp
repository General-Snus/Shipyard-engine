#pragma once
#include <vector>
#include <stack>
#include "PriorityQueue.hpp"
#include "Stack.hpp"
 
	static const int neigbours = 4;
	static const int MapWidth = 20;
	static const int MapHeight = 20;
	static const int TileCount = MapWidth * MapHeight;
	enum class Tile
	{
		Impassable,
		Passable
	};
	int Directions[] =
	{
		-MapWidth,
		1,
		-1,
		MapWidth,
		-MapWidth + 1,
		-MapWidth + -1,
		MapWidth + 1,
		MapWidth + -1
	};

	void Relax(const std::vector<Tile>& aMap,PriorityQueue<int,int>& priorityQ,int* dist,int* edgeTo,int weight)
	{
		int v = -priorityQ.Dequeue();
		for(size_t i = 0; i < neigbours; i++)
		{
			int w = v + (int)Directions[i];

			if(w < 0 || w >= TileCount)
				continue;

			if(abs((v % MapWidth + MapWidth) % MapWidth - (w % MapWidth + MapWidth) % MapWidth) == 1 || abs(v - w) == MapWidth)
			{
				if(aMap[w] != Tile::Impassable)
				{
					if(dist[w] > dist[v] + weight)
					{
						dist[w] = dist[v] + weight;
						edgeTo[w] = v;
						if(priorityQ.Contains(-w))
						{
							priorityQ.ChangePriority(-w,-dist[w]);
						}
						else
						{
							priorityQ.Enqueue(-w,-dist[w]);
						}

					}
				}
			}
		}
	}

	std::vector<int> Dijkstra(const std::vector<Tile>& aMap,int aStartIndex,int anEndIndex)
	{
		if(aStartIndex == anEndIndex)
		{
			std::vector<int> path;
			path.push_back(aStartIndex);
			return path;
		}

		int* dist = new int[TileCount];
		int* edgeTo = new int[TileCount];

		PriorityQueue<int,int> priorityQ; // This is a max queue, all input and outputs are negated to turn it into a min queue

		for(size_t i = 0; i < TileCount; i++)
		{
			dist[i] = INT_MAX;
		}
		dist[aStartIndex] = 0;
		priorityQ.Enqueue(-aStartIndex,0);

		while(priorityQ.GetSize())
		{
			Relax(aMap,priorityQ,dist,edgeTo,1);
		}


		Stack<int> pathFromLastToFirst;
		if(dist[anEndIndex] < INT_MAX)
		{
			int e = edgeTo[anEndIndex];
			pathFromLastToFirst.Push(anEndIndex);
			while(e != aStartIndex)
			{
				pathFromLastToFirst.Push(e);
				e = edgeTo[e];
			}
			pathFromLastToFirst.Push(aStartIndex);
		}
		else
		{
			delete[] dist;
			delete[] edgeTo; 
			return std::vector<int>();
		}

		std::vector<int> outArg;
		while(pathFromLastToFirst.GetSize())
		{
			outArg.push_back(pathFromLastToFirst.Pop());
		}
		delete[] dist;
		delete[] edgeTo;
		return outArg;
	} 