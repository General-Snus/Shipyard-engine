#pragma once 
#include <vector>
#include <filesystem> 
#include <array>
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

class NavMesh
{
public:
	struct Point
	{
		Vector3f Position;
	};

	struct Edge
	{
		std::array<int,2> Points;
		std::array<int,2> Neighbors;
	};

	struct Polygon
	{
		Vector3f CenterPosition;
		std::array<int,3> Edges;
	};

	struct Path
	{
		std::vector< Vector3f> myPath;
	};


	NavMesh() = default;
	~NavMesh() = default;
	void Initialize(const std::filesystem::path& PathToLoad);
	Path PathFind(Vector3f Start,Vector3f End);
	bool IsPointInside(Vector3f Point);
	void DrawDebug();
	bool RayToNavMesh(Vector3f rayOrigin,Vector3f rayVector,Polygon& outTriangle,Vector3f& outIntersectionPoint); 
	Vector3f RandomPointInNavMesh();
private:
	std::vector<int> GetNeighbors(int aIndex);

	int ClosestTriangleToPoint(Vector3f point);
	bool RayIntersectsTriangle(Vector3f rayOrigin,Vector3f rayVector,const Polygon* inTriangle,Vector3f& outIntersectionPoint);
	void Triangulate(const std::vector<int>& poly,int index,std::vector<std::vector<int>>& indices);

	 Vector3f GetPoint(int index,const Polygon& poly);

	float h(int node1,int nodeGoal);

	std::vector<int> AStar(int aStartIndex,int anEndIndex);


	std::vector<Polygon> myPolygons;
	std::vector<Edge> myEdges;
	std::vector<Vector3f> myPoints;

	bool IsReady;
	//Tga::LineMultiPrimitive myDrawnMesh;
};