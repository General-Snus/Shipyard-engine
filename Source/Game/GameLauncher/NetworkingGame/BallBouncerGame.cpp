#include "ShipyardEngine.pch.h"
#include "BallBouncerGame.h"
#include "Engine\AssetManager\GameResourcesLoader.h"
#include "Engine\AssetManager\ComponentSystem\Components\Network\NetworkSync.h"

GameObject BallEradicationGame::MakeBall(Vector3f position)
{
	const auto startSpeed = 10.f;
	GameObject ball = GameObject::Create("Ball");
	auto& renderer = ball.AddComponent<MeshRenderer>("Models/BallEradicationGame/Sphere.fbx");
	ball.transform().SetPosition(position); 

	auto& collider = ball.AddComponent<Collider>();
	ball.AddComponent<BallTag>();
	ball.AddComponent<NetworkObject>();
	ball.AddComponent<NetworkTransform>();
	auto& kinematic = ball.AddComponent<cPhysics_Kinematic>();
	
	kinematic.ph_velocity =
		Vector3f(
			Math::RandomEngine::randomInRange(-1.0f,1.0f),
			0,
			Math::RandomEngine::randomInRange(-1.0f,1.0f)
		).GetNormalized() * startSpeed;

	collider.SetColliderType<ColliderAssetSphere>();

	if(const auto mat = Resources.ForceLoad<Material>("TreeMaterial")) {
		mat->SetColor(ColorManagerInstance.GetColor("Red"));
		renderer.SetMaterial(mat);
	}
	return ball;
}
 

void BallEradicationGame::MakeArena(Vector3f position,Vector3f extent)
{
	GameObject arenaParent = GameObject::Create("Arena");
	arenaParent.transform().SetPosition(position);

	GameObject arenaFloor = GameObject::Create("ArenaFloor");
	arenaFloor.AddComponent<MeshRenderer>("Models/Cube.fbx");
	Vector3f floorPosition = {position.x,position.y -1,position.z};
	arenaFloor.transform().SetPosition(floorPosition);
	arenaFloor.transform().SetScale(2.f*extent);
	arenaFloor.transform().SetParent(arenaParent.transform());


	{
		GameObject arenaWall = GameObject::Create("ArenaWall");
		arenaWall.AddComponent<MeshRenderer>("Models/Cube.fbx");
		Vector3f arenaWallPosition = {position.x+extent.x,position.y,position.z};
		arenaWall.transform().SetPosition(arenaWallPosition);
		arenaWall.transform().SetScale(1,extent.y,2*extent.z);
		arenaWall.transform().SetParent(arenaParent.transform());
	}

	{
		GameObject arenaWall = GameObject::Create("ArenaWall");
		arenaWall.AddComponent<MeshRenderer>("Models/Cube.fbx");
		Vector3f arenaWallPosition = {position.x-extent.x,position.y,position.z};
		arenaWall.transform().SetPosition(arenaWallPosition);
		arenaWall.transform().SetScale(1,extent.y,2*extent.z);
		arenaWall.transform().SetParent(arenaParent.transform());
	}

	{
		GameObject arenaWall = GameObject::Create("ArenaWall");
		arenaWall.AddComponent<MeshRenderer>("Models/Cube.fbx");
		Vector3f arenaWallPosition = {position.x,position.y,position.z+extent.z};
		arenaWall.transform().SetPosition(arenaWallPosition);
		arenaWall.transform().SetScale(2*extent.x,extent.y,1);
		arenaWall.transform().SetParent(arenaParent.transform());
	}

	{
		GameObject arenaWall = GameObject::Create("ArenaWall");
		arenaWall.AddComponent<MeshRenderer>("Models/Cube.fbx");
		Vector3f arenaWallPosition = {position.x,position.y,position.z-extent.z};
		arenaWall.transform().SetPosition(arenaWallPosition);
		arenaWall.transform().SetScale(2*extent.x,extent.y,1);
		arenaWall.transform().SetParent(arenaParent.transform());
	}
}
