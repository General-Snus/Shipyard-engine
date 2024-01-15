#include "AssetManager.pch.h"
#include <Editor/Editor/Core/Editor.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>
#include "../Collider.h"

cCollider::cCollider(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myCollider = std::make_shared<ColliderAssetAABB>();
}

cCollider::cCollider(const unsigned int anOwnerId,const std::filesystem::path aPath) : Component(anOwnerId)
{
	myCollider = std::make_shared<ColliderAssetAABB>();
}

void cCollider::Update()
{
	const AABB3D<float>& thisCollider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();

	for (const auto& otherColliders : GameObjectManager::Get().GetAllComponents<cCollider>())
	{
		if (otherColliders.GetOwner() == myOwnerID)
		{
			continue;
		}

		const auto& collider = otherColliders.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
		if (IntersectionAABB(thisCollider,collider))
		{
			GameObjectManager::Get().CollidedWith(myOwnerID,otherColliders.GetOwner());
			return;
		}
	}
}

Vector3f cCollider::GetClosestPosition(Vector3f position) const
{
	return GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().ClosestPoint(position);
}

Vector3f roundToBasis(Vector3f input)
{
	Vector3f output;
	output.x = round(input.x);
	output.y = round(input.y);
	output.z = round(input.z);
	return output;
}

Vector3f cCollider::GetNormalToward(Vector3f position) const
{
	const Transform& otherObj = GetComponent<Transform>();
	const auto collider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
	Vector3f norm = roundToBasis((collider.ClosestPoint(position) - otherObj.GetPosition()).GetNormalized());
	return norm;
}

void cCollider::Render()
{
	if (Editor::Get().GetApplicationState().drawDebugLines == false)
	{
		return;
	}

	if (const auto* transform = TryGetComponent<Transform>())
	{
		myCollider->RenderDebugLines(*transform);
	}
}

void cCollider::OnSiblingChanged(const std::type_info* SourceClass)
{
	if (SourceClass == &typeid(Transform)) // Transform dirty
	{
		const Transform& transform = GetComponent<Transform>();
		GetColliderAssetOfType<ColliderAssetAABB>()->UpdateWithTransform(transform.GetTransform());
		GetGameObject().OnSiblingChanged(&typeid(cCollider));
	}
}
