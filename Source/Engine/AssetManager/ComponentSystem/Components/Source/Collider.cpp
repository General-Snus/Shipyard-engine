#include "AssetManager.pch.h"

#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include <Editor/Editor/Core/Editor.h>
#include <Engine/PersistentSystems/System/Colission/OnCollision.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>
#include "Editor/Editor/Core/ApplicationState.h"

Collider::Collider(const SY::UUID anOwnerId, GameObjectManager* aManager) : Component(anOwnerId, aManager)
{
	if (const auto renderer = TryGetComponent<MeshRenderer>())
	{
		myCollider = std::make_shared<ColliderAssetAABB>(renderer->GetBoundingBox());
	}
	else
	{
		myCollider = std::make_shared<ColliderAssetAABB>();
	}
}

Collider::Collider(const SY::UUID anOwnerId, GameObjectManager* aManager, const std::filesystem::path& aPath)
	: Component(anOwnerId, aManager)
{
	aPath;

	if (const auto renderer = TryGetComponent<MeshRenderer>())
	{
		myCollider = std::make_shared<ColliderAssetAABB>(renderer->GetBoundingBox());
	}
	else
	{
		myCollider = std::make_shared<ColliderAssetAABB>();
	}
}

void Collider::Destroy()
{
	OPTICK_EVENT();
	SystemCollection::GetSystem<CollisionChecks>()->RemoveCollisions(GetOwner());
}

void Collider::Update()
{
	OPTICK_EVENT();
	if (drawDebugLines && myCollider)
	{
		myCollider->RenderDebugLines(transform());
	}
}

Vector3f Collider::GetClosestPosition(Vector3f position) const
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

Vector3f Collider::GetNormalToward(Vector3f position) const
{
	const Transform& otherObj = GetComponent<Transform>();
	const auto       collider = GetColliderAssetOfType<ColliderAssetAABB>()->GetAABB();
	const Vector3f   norm = roundToBasis((collider.ClosestPoint(position) - otherObj.GetPosition()).GetNormalized());
	return norm;
}

void Collider::OnSiblingChanged(const std::type_info* SourceClass)
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
		GetGameObject().OnSiblingChanged(&typeid(Collider));
	}
}

bool Collider::InspectorView()
{
	if (!Component::InspectorView())
	{
		return false;
	}
	Reflect<Collider>();
	if (myCollider)
	{
		myCollider->InspectorView();

		if (!drawDebugLines) { myCollider->RenderDebugLines(transform()); }
	}
	return true;
}
