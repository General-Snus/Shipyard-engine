#include "AssetManager.pch.h" 
#include <Editor/Editor/Core/Editor.h> 

cCollider::cCollider(const unsigned int anOwnerId) : Component(anOwnerId)
{
	myCollider = std::make_shared<ColliderAssetAABB>();
}

cCollider::cCollider(const unsigned int anOwnerId,const std::filesystem::path& aPath) : Component(anOwnerId)
{
	aPath;
	myCollider = std::make_shared<ColliderAssetAABB>();
}



void cCollider::Update()
{
	OPTICK_EVENT();
	//const AABB3D<float>& thisCollider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
	//for (const auto& otherColliders : GameObjectManager::Get().GetAllComponents<cCollider>())
	//{
	//	if (otherColliders.GetOwner() == myOwnerID)
	//	{
	//		continue;
	//	}
	//
	//	const auto& collider = otherColliders.GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
	//	if (IntersectionAABB(thisCollider,collider))
	//	{
	//		GameObjectManager::Get().CollidedWith(myOwnerID,otherColliders.GetOwner());
	//		return;
	//	}
	//}
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
	OPTICK_EVENT();
	if (ApplicationState::drawDebugLines == false)
	{
		return;
	}

	if (auto* transform = TryGetComponent<Transform>())
	{
		myCollider->RenderDebugLines(*transform);
	}
}

void cCollider::OnSiblingChanged(const std::type_info* SourceClass)
{
	OPTICK_EVENT();
	if (SourceClass == &typeid(Transform)) // Transform dirty
	{
		Transform& transform = GetComponent<Transform>();
		const auto colliderType = myCollider->GetColliderType();
		switch (colliderType)
		{
		case eColliderType::AABB:
			GetColliderAssetOfType<ColliderAssetAABB>()->UpdateWithTransform(transform.GetTransform());
			break;
		case eColliderType::SPHERE:
			break;
		case eColliderType::CONVEX:
			break;
		case eColliderType::PLANAR:
			break;
		default:
			break;
		}
		GetGameObject().OnSiblingChanged(&typeid(cCollider));
	}
}
void cCollider::InspectorView()
{
	ImGui::Text("cCollider");
}