#include "AssetManager.pch.h"
#include "../Collider.h"
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>

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
	const Transform& transform = GetComponent<Transform>();
	const AABB3D<float>& thisCollider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().UpdateWithTransform(transform.GetTransform());

	for(auto& i : GameObjectManager::Get().GetAllComponents<cCollider>())
	{
		if(i.GetOwner() == myOwnerID)
		{
			continue;
		}

		const Transform& otherObj = i.GetComponent<Transform>();
		auto collider = i.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().UpdateWithTransform(otherObj.GetTransform());;
		if(IntersectionAABB(thisCollider,collider))
		{
			GameObjectManager::Get().CollidedWith(myOwnerID,i.GetOwner());
			return;
		}
	}
}

Vector3f cCollider::GetClosestPosition(Vector3f position) const
{
	const Transform& otherObj = GetComponent<Transform>();
	const auto collider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().UpdateWithTransform(otherObj.GetTransform());;

	return collider.ClosestPoint(position);
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
	const auto collider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().UpdateWithTransform(otherObj.GetTransform());
	Vector3f norm = roundToBasis((collider.ClosestPoint(position) - otherObj.GetPosition()).GetNormalized());
	return norm;
}

void cCollider::Render()
{
#ifdef _DEBUGDRAW  
	if(const auto* transform = TryGetComponent<Transform>())
	{
		myCollider->RenderDebugLines(*transform);
	}
#endif // _DEBUGDRAW 
}
