#pragma once
#include <vector>
#include <DataStructures/Stack.hpp>
#include <DataStructures/HashMap.hpp>
#include <DataStructures/PriorityQueue.hpp>
#include <LinearAlgebra/Vector2.hpp>

namespace Assignment7Astar { 

#define mod(x,y) (x% y + y) % y

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


	inline float h(int node1, int nodeGoal)
	{
		node1; nodeGoal;
		 Vector2f node1Pos = { static_cast<float>(mod(node1,MapWidth)),		 static_cast<float>(floorf((float)node1 / (float)MapWidth)) };
		 Vector2f nodeGoalPos = { static_cast<float>(mod(nodeGoal,MapWidth)),  static_cast<float>(floorf((float)nodeGoal / (float)MapWidth)) };

		int d = 1;

		return static_cast<float>(d * (std::abs(node1Pos.x - nodeGoalPos.x) + std::abs(node1Pos.y - nodeGoalPos.y))); 
	}


	inline std::vector<int> AStar(const std::vector<Tile>& aMap, int aStartIndex, int anEndIndex)
	{
		if (aStartIndex == anEndIndex)
		{
			std::vector<int> path;
			path.push_back(aStartIndex);
			return path;
		}

		PriorityQueue<int, float> priorityQ; // This is a max queue, all input and outputs are negated to turn it into a min queue
		HashMap<int, float> gscore(TileCount);
		HashMap<int, float> fscore(TileCount);
		HashMap<int, int> previous(TileCount);


		for (int i = 0; i < TileCount; i++)
		{
			gscore.Insert(i, static_cast<float>(INT_MAX));
			fscore.Insert(i, static_cast<float>(INT_MAX));
		}
		gscore.Insert(aStartIndex, 0);
		fscore.Insert(aStartIndex, h(aStartIndex, anEndIndex));
		priorityQ.Enqueue(-aStartIndex, 0.0f);
		int current = 0;
		int counter = 0;
		while (priorityQ.GetSize() != 0 && counter < INT_MAX)
		{
			counter++;
			current = -priorityQ.Dequeue();
			if (current == anEndIndex)
			{
				Stack<int> pathFromLastToFirst; 
				while (previous.Get(current) != nullptr)
				{
					pathFromLastToFirst.Push(current);
					current = *previous.Get(current);
				}
				pathFromLastToFirst.Push(aStartIndex);

				std::vector<int> outArg;
				while (pathFromLastToFirst.GetSize())
				{
					outArg.push_back(pathFromLastToFirst.Pop());
				}
				return outArg;
			}

			for (size_t i = 0; i < neigbours; i++)
			{
				int w = current + (int)Directions[i];

				if (w < 0 || w >= TileCount)
					continue;

				if ((abs(mod(current, MapWidth) - mod(w, MapWidth)) == 1 || abs(current - w) == MapWidth))
				{
					if (aMap[w] != Tile::Impassable)
					{
						const int weight = 10;
						float totalWeightToGetToCurrent = *gscore.Get(current) + weight;
						if (totalWeightToGetToCurrent < *gscore.Get(w))
						{
							previous.Insert(w, current);
							gscore.Insert(w, totalWeightToGetToCurrent);
							fscore.Insert(w, totalWeightToGetToCurrent + h(w, anEndIndex));

							if (!priorityQ.Contains(-w))
							{
								priorityQ.Enqueue(-w, -*fscore.Get(w));
							}
						}
					}
				}
			}
		}
		return std::vector<int>();
	}
}