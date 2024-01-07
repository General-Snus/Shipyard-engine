#include "../NavMesh.h"
#include <sstream>
#include <fstream>
#include <string> 
#include <random>

#include <Tools/Utilities/DataStructures/PriorityQueue.hpp>
#include <Tools/Utilities/DataStructures/Stack.hpp>
#include <Tools/Utilities/DataStructures/HashMap.hpp> 
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

void NavMesh::Triangulate(const std::vector<int>& poly,int index,std::vector<std::vector<int>>& indices)
{
	std::vector<int> newPoly;
	newPoly.resize(3);
	int newIndex = index - 1;
	for(int i = index; i < 3 + index; i++)
	{
		int a = i % 3;
		int b = i % poly.size();
		newPoly[a] = poly[b];
		newIndex++;
	}
	if(newIndex < poly.size())
	{
		Triangulate(poly,newIndex,indices);
	}
	indices.push_back(newPoly);
}

void NavMesh::Initialize(const std::filesystem::path& PathToLoad)
{
	std::ifstream InpFStream;
	InpFStream.open(PathToLoad);
	if(!InpFStream.is_open()) return;

	auto readLine = new char[5000];
	std::vector<std::vector<int>> indices;

	while(!InpFStream.eof())
	{
		std::stringstream inputString;
		InpFStream.getline(readLine,5000);

		if(readLine[0] == 'v')
		{
			Vector4f position;
			inputString.str(&readLine[2]);
			std::string str;
			inputString >> str;
			position.x = std::stof(str);

			inputString >> str;
			position.z = std::stof(str);

			inputString >> str;
			position.y = std::stof(str);


			position.w = 1;
			Vector3f newPoint;
			newPoint = {position.x,position.y,position.z};
			myPoints.push_back(newPoint);
		}
		else if(readLine[0] == 'f')
		{
			std::string check = readLine;

			inputString.str(&readLine[2]);
			std::vector<int> poly;

			while(!inputString.eof())
			{
				std::string value;
				inputString >> value;
				poly.push_back(std::stoi(value) - 1);
			}
			if(poly.size() > 3)
			{
				Triangulate(poly,0,indices);
				continue;
			}
			indices.push_back(poly);
		}
	}

	InpFStream.close();

	for(auto& v : indices)
	{
		Polygon polygon;
		Edge newEdge{};

		newEdge.Neighbors[0] = -1;
		newEdge.Neighbors[1] = -1;

		newEdge.Points[0] = v[0];
		newEdge.Points[1] = v[1];
		myEdges.push_back(newEdge);
		polygon.Edges[0] = static_cast<int>(myEdges.size() - 1);


		newEdge.Points[0] = v[1];
		newEdge.Points[1] = v[2];
		myEdges.push_back(newEdge);
		polygon.Edges[1] = static_cast<int>(myEdges.size() - 1);

		newEdge.Points[0] = v[2];
		newEdge.Points[1] = v[0];
		myEdges.push_back(newEdge);
		polygon.Edges[2] = static_cast<int>(myEdges.size() - 1);

		Vector3f center = myPoints[v[0]] + myPoints[v[1]] + myPoints[v[2]];
		center.x /= 3;
		center.y /= 3;
		center.z /= 3;

		polygon.CenterPosition = center;
		myPolygons.push_back(polygon);
	}
	auto size = static_cast<int> (myPolygons.size());
	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			if(i != j)
			{
				for(auto& polyAEdges : myPolygons[i].Edges)
				{
					for(auto& polyBEdges : myPolygons[j].Edges)
					{
						if(myEdges[polyAEdges].Points[0] == myEdges[polyBEdges].Points[1] &&
							myEdges[polyAEdges].Points[1] == myEdges[polyBEdges].Points[0]
							)
						{
							myEdges[polyAEdges].Neighbors[0] = j;
							myEdges[polyAEdges].Neighbors[1] = i;

							myEdges[polyBEdges].Neighbors[0] = j;
							myEdges[polyBEdges].Neighbors[1] = i;
						}
					}
				}
			}
		}
	}

	//myDrawnMesh.count = static_cast<unsigned int> (myEdges.size());
	//myDrawnMesh.colors = new  Vector3f[myEdges.size()];
	//myDrawnMesh.fromPositions = new  Vector3f[myEdges.size()];
	//myDrawnMesh.toPositions = new  Vector3f[myEdges.size()];
	int index = 0;
	for(auto& i : myEdges)
	{
		i;
		//myDrawnMesh.fromPositions[index] = myPoints[i.Points[0]];
		//myDrawnMesh.toPositions[index] = myPoints[i.Points[1]];
		//myDrawnMesh.colors[index] = Vector4f(1,1,1,1);
		index++;
	}
	delete[] readLine;
	IsReady = true;
}

NavMesh::Path NavMesh::PathFind(Vector3f Start,Vector3f End)
// Hur fan hittar jag exakt triangel i punkt i 3d space? måste vara i varje triangels space? Du är aldrig rakt på en 2d struktur...
{
	if(!IsReady) return Path();

	int startPoly = ClosestTriangleToPoint(Start);
	int endPoly = ClosestTriangleToPoint(End);
	std::vector<int> intPath = AStar(startPoly,endPoly);

	if(!intPath.size())
	{
		return Path();
	}

	Path path;
	path.myPath.push_back(Start);

	for(auto const& i : intPath)
	{
		path.myPath.push_back(myPolygons[i].CenterPosition);
	}
	path.myPath.push_back(End);

	return path;
}

bool NavMesh::IsPointInside(Vector3f Point)
{
	return false;
	Point;
}

void NavMesh::DrawDebug()
{
	if(!IsReady) return;
	//Tga::Engine::GetInstance()->GetGraphicsEngine().GetCustomShapeDrawer();
	//Tga::LineDrawer& lineDrawer = Tga::Engine::GetInstance()->GetGraphicsEngine().GetLineDrawer();
	//lineDrawer.Draw(myDrawnMesh); 
	//Tga::DebugDrawer& debugDrawer = Tga::Engine::GetInstance()->GetDebugDrawer();

	for(const auto& i : myPolygons)
	{
		//debugDrawer.DrawCircle({i.CenterPosition.x,i.CenterPosition.y},1);
		for(auto& edge : i.Edges)
		{
			int index = myEdges[edge].Neighbors[0];
			if(index == -1)
			{
				continue;
			}
			const Vector2f from = {i.CenterPosition.x,i.CenterPosition.y};
			const Vector2f to = {myPolygons[index].CenterPosition.x,myPolygons[index].CenterPosition.y};
			const Vector3f grey = {0.25,0.25,0.25};

			//debugDrawer.DrawArrow(from,to,grey,3.0f);
		}
	}
}

bool NavMesh::RayToNavMesh(Vector3f rayOrigin,Vector3f rayVector,Polygon& outTriangle,Vector3f& outIntersectionPoint)
{
	for(const auto& i : myPolygons)
	{
		if(RayIntersectsTriangle(rayOrigin,rayVector,&i,outIntersectionPoint))
		{
			outTriangle = i;
			return true;
		}
	}

	return false;
}

Vector3f NavMesh::RandomPointInNavMesh()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution dis(0,static_cast<int>(myPolygons.size()) - 1);
	return myPolygons[dis(gen)].CenterPosition;
}

std::vector<int> NavMesh::GetNeighbors(int aIndex)
{
	std::vector<int> allNeighbors;
	for(auto const& i : myPolygons[aIndex].Edges)
	{
		for(size_t j = 0; j < 2; j++)
		{
			if(myEdges[i].Neighbors[j] != aIndex && myEdges[i].Neighbors[j] != -1)
			{
				allNeighbors.push_back(myEdges[i].Neighbors[j]);
			}
		}
	}

	return allNeighbors;
}

int NavMesh::ClosestTriangleToPoint(Vector3f point)
{
	auto closestDistance = (float)INT32_MAX;
	int closestPoly = -1;
	for(int i = 0; i < myPolygons.size(); i++)
	{
		const float newDistance = (myPolygons[i].CenterPosition - point).LengthSqr();
		if(newDistance < closestDistance)
		{
			closestDistance = newDistance;
			closestPoly = i;
		}
	}
	return closestPoly;
}

bool NavMesh::RayIntersectsTriangle(Vector3f rayOrigin,Vector3f rayVector,const Polygon* inTriangle,Vector3f& outIntersectionPoint)
{
	const float EPSILON = 0.0000001f;
	Vector3f vertex0 = GetPoint(0,*inTriangle);
	Vector3f vertex1 = GetPoint(1,*inTriangle);
	Vector3f vertex2 = GetPoint(2,*inTriangle);

	Vector3f edge1;
	Vector3f edge2;
	Vector3f h;
	Vector3f s;
	Vector3f q;
	float a;
	float f;
	float u;
	float v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = rayVector.Cross(edge2);
	a = edge1.Dot(h);

	if(a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.

	f = 1.0f / a;
	s = rayOrigin - vertex0;
	u = f * s.Dot(h);

	if(u < 0.0 || u > 1.0)
		return false;

	q = s.Cross(edge1);
	v = f * rayVector.Dot(q);

	if(v < 0.0 || u + v > 1.0)
		return false;

	float t = f * edge2.Dot(q);

	if(t > EPSILON)
	{
		outIntersectionPoint = rayOrigin + rayVector * t;
		return true;
	}
	else
		return false;

}

Vector3f NavMesh::GetPoint(int index,const Polygon& poly)
{
	return  myPoints[myEdges[poly.Edges[index]].Points[0]];
}

#pragma region Asstar

#define mod(x,y) (x% y + y) % y
inline float NavMesh::h(int node1,int nodeGoal)
{
	//For now we go polycenter then closest edge point
	Vector3f node1Pos = myPolygons[node1].CenterPosition;
	Vector3f nodeGoalPos = myPolygons[nodeGoal].CenterPosition;

	float d = 1;

	return d * (node1Pos - nodeGoalPos).LengthSqr();
}


inline std::vector<int> NavMesh::AStar(int aStartIndex,int anEndIndex)
{
	if(aStartIndex == anEndIndex)
	{
		std::vector<int> path;
		path.push_back(aStartIndex);
		return path;
	}

	PriorityQueue<int,float> priorityQ; // This is a max queue, all input and outputs are negated to turn it into a min queue
	HashMap<int,float> gscore(static_cast<int>(myPolygons.size()));
	HashMap<int,float> fscore(static_cast<int>(myPolygons.size()));
	HashMap<int,int> previous(static_cast<int>(myPolygons.size()));


	for(int i = 0; i < myPolygons.size(); i++)
	{
		gscore.Insert(i,static_cast<float>(INT_MAX));
		fscore.Insert(i,static_cast<float>(INT_MAX));
	}

	gscore.Insert(aStartIndex,0);
	fscore.Insert(aStartIndex,h(aStartIndex,anEndIndex));
	priorityQ.Enqueue(-aStartIndex,0.0f);

	int current = 0;
	int counter = 0;
	while(priorityQ.GetSize() != 0 && counter < INT_MAX)
	{
		counter++;
		current = -priorityQ.Dequeue();
		if(current == anEndIndex)
		{
			Stack<int> pathFromLastToFirst;
			while(previous.Get(current) != nullptr)
			{
				pathFromLastToFirst.Push(current);
				current = *previous.Get(current);
			}

			pathFromLastToFirst.Push(aStartIndex);

			std::vector<int> outArg;
			while(pathFromLastToFirst.GetSize())
			{
				outArg.push_back(pathFromLastToFirst.Pop());
			}
			return outArg;
		}

		for(auto const& nextPolygon : GetNeighbors(current))
		{
			const float weight = h(nextPolygon,current);
			float totalWeightToGetToCurrent = *gscore.Get(current) + weight;
			if(totalWeightToGetToCurrent < *gscore.Get(nextPolygon))
			{
				previous.Insert(nextPolygon,current);
				gscore.Insert(nextPolygon,totalWeightToGetToCurrent);
				fscore.Insert(nextPolygon,totalWeightToGetToCurrent + h(nextPolygon,anEndIndex));

				if(!priorityQ.Contains(-nextPolygon))
				{
					priorityQ.Enqueue(-nextPolygon,-*fscore.Get(nextPolygon));
				}
			}
		}
	}
	return std::vector<int>();
}
#pragma endregion