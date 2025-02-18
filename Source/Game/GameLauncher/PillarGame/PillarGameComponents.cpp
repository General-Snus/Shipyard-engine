#include "ShipyardEngine.pch.h"

#include "PillarGameComponents.h"
#include <Engine/AssetManager/GameResourcesLoader.h>

GameObject SpawnPillar(Vector3f base) {
	GameObject pillar = GameObject::Create("Pillar");
	auto& renderer = pillar.AddComponent<MeshRenderer>("Models/PillarClimb/Pillar.fbx");
	pillar.transform().SetPosition(base);
	auto& collider = pillar.AddComponent<Collider>();

	collider.GetColliderAssetOfType<ColliderAssetBox>()->box().GetExtent().y = 25.0f;
	collider.GetColliderAssetOfType<ColliderAssetBox>()->box().GetCenter().y = 25.0f;
	pillar.AddComponent<cPhysXStaticBody>();

	if(const auto mat = Resources.ForceLoad<Material>("TreeMaterial")) {
		mat->SetColor(ColorManagerInstance.GetColor("Brown"));
		renderer.SetMaterial(mat);
	}
	return pillar;
}

void SpawnGround(Vector3f base) {
	GameObject ground = GameObject::Create("ground");
	auto& renderer = ground.AddComponent<MeshRenderer>("Models/Cube.fbx");
	ground.transform().SetPosition(base);
	ground.transform().SetScale(100,0.1f,100.f);

	if(const auto mat = Resources.ForceLoad<Material>("GroundMaterial")) {
		mat->SetColor(ColorManagerInstance.GetColor("Olive"));
		renderer.SetMaterial(mat);
	}
	ground.AddComponent<cPhysXStaticBody>();
}

void SpawnHooks(int amount,float radius,GameObject parent) {
	for(int i = 0; i < amount; ++i) {
		GameObject attachment = GameObject::Create(std::format("Hook_{}",i));
		attachment.transform().SetParent(parent.transform());
		attachment.AddComponent<HookComponent>();

		auto& renderer = attachment.AddComponent<MeshRenderer>("Models/PillarClimb/AttachmentPost.fbx");

		if(const auto mat = Resources.ForceLoad<Material>("AttachmentPostMaterial")) {
			mat->SetColor(ColorManagerInstance.GetColor("Green"));
			renderer.SetMaterial(mat);
		}

		auto& ref = attachment.AddComponent<Collider>();
		ref.GetColliderAssetOfType<ColliderAssetBox>()->box().GetExtent().z = 0.1f;
		attachment.AddComponent<cPhysXStaticBody>();
		auto directionOffset =
			Vector3f(Math::RandomEngine::randomInRange(-1.0f,1.0f),0,Math::RandomEngine::randomInRange(-1.0f,1.0f))
			.GetNormalized();

		auto position = Math::GlobalUp * .5f + directionOffset * radius * 1.05f;
		position.y = static_cast<float>(i);

		attachment.transform().LookAt(directionOffset * 10.f);
		attachment.transform().SetRotation(0,attachment.transform().euler().y,0);
		attachment.transform().SetPosition(position);
	}
}

void SpawnPlayer(int id,float radius,GameObject parent) {
	id;
	GameObject player = GameObject::Create("Player");
	Scene::activeManager().SetLastGOAsPlayer();
	player.transform().SetParent(parent.transform());

	const auto directionOffset =
		Vector3f(
			Math::RandomEngine::randomInRange(-1.0f,1.0f),
			0,
			Math::RandomEngine::randomInRange(-1.0f,1.0f)
		).GetNormalized();

	player.transform().LookAt(directionOffset * 10.0f);
	player.transform().SetPosition(0,0.5f,0);

	GameObject playerModel = GameObject::Create("PlayerModel");
	playerModel.AddComponent<MeshRenderer>("Models/PillarClimb/Player.fbx");
	playerModel.transform().SetParent(player.transform());
	playerModel.transform().SetPosition(0,0,-radius * 1.25f);
	playerModel.transform().SetScale(.1f);

	/*GameObject test = GameObject::Create("PlayerBackCOllider");
	test.transform().SetParent(player.transform());
	test.transform().SetPosition(0,0,-radius * 2.25f);
	test.AddComponent<Collider>();
	test.AddComponent<cPhysXStaticBody>();*/


	GameObject camera = GameObject::Create("Player Camera");
	camera.AddComponent<Camera>();
	Scene::activeManager().SetLastGOAsCamera();
	camera.AddComponent<MeshRenderer>("Models/Camera/Camera.fbx");

	camera.transform().SetParent(player.transform());
	camera.transform().SetPosition(0,0,-2);
	player.AddComponent<PlayerComponent>();
}
