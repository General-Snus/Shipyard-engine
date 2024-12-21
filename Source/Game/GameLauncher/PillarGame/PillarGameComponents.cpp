#include "ShipyardEngine.pch.h"

#include "PillarGameComponents.h"
#include <Engine/AssetManager/GameResourcesLoader.h>

void SpawnPillar(Vector3f base)
{
	GameObject pillar = GameObject::Create("Pillar");
	auto&      renderer = pillar.AddComponent<MeshRenderer>("Models/PillarClimb/Pillar.fbx");
	pillar.transform().SetPosition(base);

	if (const auto mat = Resources.ForceLoad<Material>("TreeMaterial"))
	{
		mat->SetColor(ColorManagerInstance.GetColor("Brown"));
		renderer.SetMaterial(mat);
	}
}

void SpawnGround(Vector3f base)
{
	GameObject ground = GameObject::Create("ground");
	auto&      renderer = ground.AddComponent<MeshRenderer>("Models/Cube.fbx");
	ground.transform().SetPosition(base);
	ground.transform().SetScale(100, 0.1f, 100.f);

	if (const auto mat = Resources.ForceLoad<Material>("GroundMaterial"))
	{
		mat->SetColor(ColorManagerInstance.GetColor("Olive"));
		renderer.SetMaterial(mat);
	}
	ground.AddComponent<cPhysXStaticBody>();
}

void SpawnHooks(int amount, float radius, Vector3f base)
{
	for (int i = 0; i < amount; ++i)
	{
		GameObject attachment = GameObject::Create(std::format("Hook_{}", i));
		attachment.AddComponent<HookComponent>();

		auto& renderer = attachment.AddComponent<MeshRenderer>("Models/PillarClimb/AttachmentPost.fbx");

		if (const auto mat = Resources.ForceLoad<Material>("AttachmentPostMaterial"))
		{
			mat->SetColor(ColorManagerInstance.GetColor("Green"));
			renderer.SetMaterial(mat);
		}

		attachment.AddComponent<Collider>();
		attachment.AddComponent<cPhysXStaticBody>();
		auto directionOffset =
			Vector3f(RandomEngine::randomInRange(-1.0f, 1.0f), 0, RandomEngine::randomInRange(-1.0f, 1.0f))
			.GetNormalized();

		auto position = base + directionOffset * radius * 1.25f;
		position.y = static_cast<float>(i);

		attachment.transform().LookAt(directionOffset * 10.f);
		attachment.transform().SetRotation(0, attachment.transform().GetRotation().y, 0);
		attachment.transform().SetPosition(position);
	}
}

void SpawnPlayer(int id, float radius, Vector3f base)
{
	id;
	GameObject player = GameObject::Create("Player");
	Scene::activeManager().SetLastGOAsPlayer();

	const auto directionOffset =
		Vector3f(RandomEngine::randomInRange(-1.0f, 1.0f), 0, RandomEngine::randomInRange(-1.0f, 1.0f)).GetNormalized();
	player.transform().LookAt(directionOffset * 10.0f);
	player.transform().SetPosition(base);
	player.AddComponent<cPhysXStaticBody>();

	GameObject playerModel = GameObject::Create("PlayerModel");
	playerModel.AddComponent<MeshRenderer>("Models/PillarClimb/Player.fbx");
	playerModel.transform().SetParent(player.transform());
	playerModel.transform().SetPosition(0, 0, -radius * 1.25f);
	playerModel.transform().SetScale(.1f);


	GameObject camera = GameObject::Create("Player Camera");
	camera.AddComponent<Camera>();
	Scene::activeManager().SetLastGOAsCamera();
	camera.AddComponent<MeshRenderer>("Models/Camera/Camera.fbx");

	camera.transform().SetParent(player.transform());
	camera.transform().SetPosition(0, 0, -2);
	player.AddComponent<PlayerComponent>();
}
