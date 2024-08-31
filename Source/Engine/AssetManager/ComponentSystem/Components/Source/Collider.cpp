#include "Engine/AssetManager/AssetManager.pch.h"

#include "Editor/Editor/Core/ApplicationState.h"
#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include <Editor/Editor/Core/Editor.h>
#include <Engine/PersistentSystems/System/Colission/OnCollision.h>
#include <Tools/Utilities/LinearAlgebra/Intersection.hpp>

cCollider::cCollider(const SY::UUID anOwnerId, GameObjectManager *aManager) : Component(anOwnerId, aManager)
{
    myCollider = std::make_shared<ColliderAssetAABB>();
}

cCollider::cCollider(const SY::UUID anOwnerId, GameObjectManager *aManager, const std::filesystem::path &aPath)
    : Component(anOwnerId, aManager)
{
    aPath;
    myCollider = std::make_shared<ColliderAssetAABB>();
}

void cCollider::Destroy()
{
    OPTICK_EVENT();
    CollisionChecks::RemoveCollisions(GetOwner());
}

void cCollider::Update()
{
    OPTICK_EVENT();
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
    const Transform &otherObj = GetComponent<Transform>();
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

    if (auto *transform = TryGetComponent<Transform>())
    {
        myCollider->RenderDebugLines(*transform);
    }
}

void cCollider::OnSiblingChanged(const std::type_info *SourceClass)
{
    OPTICK_EVENT();
    if (SourceClass == &typeid(Transform)) // Transform dirty
    {
        Transform &transform = GetComponent<Transform>();
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
bool cCollider::InspectorView()
{
    if (!Component::InspectorView())
    {
        return false;
    }
    Reflect<cCollider>();
    if (myCollider)
    {
        myCollider->InspectorView();
    }
    return true;
}
