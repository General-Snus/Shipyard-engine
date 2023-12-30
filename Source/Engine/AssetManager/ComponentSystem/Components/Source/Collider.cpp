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

Vector3f cCollider::GetClosestPosition(Vector3f position)
{
	const Transform& otherObj = GetComponent<Transform>();
	const auto collider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB().UpdateWithTransform(otherObj.GetTransform());;

	return collider.ClosestPoint(position);
}

void cCollider::CreateAABB(const AABB3D<float>& rf)
{
	myCollider = std::make_shared<ColliderAssetAABB>(rf);
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
