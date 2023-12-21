#include "AssetManager.pch.h"
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
