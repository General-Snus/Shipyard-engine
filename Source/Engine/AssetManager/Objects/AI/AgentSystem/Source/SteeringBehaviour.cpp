#include "AssetManager.pch.h"

#include "../SteeringBehaviour.h"
#include <Engine/AssetManager/ComponentSystem/Components/Physics/cPhysics_Kinematic.h>
#include <Tools/Utilities/LinearAlgebra/Quaternions.hpp>

#include "Engine/AssetManager/ComponentSystem/Components/Collider.h"
#include "Engine/PersistentSystems/Scene.h"

void SteeringBehaviour::LookAt(cPhysics_Kinematic *kinematic, Vector3f TargetDirection, Vector3f forward,
                               float strength)
{
    OPTICK_EVENT();
    TargetDirection.Normalize();
    forward.Normalize();

    auto angles = Quaternionf::RotationFromTo(forward, TargetDirection).GetEulerAngles() * RAD_TO_DEG;

    // Scrub data that can accidentaly enter the system from my bad non quaternion math
    angles.x = 0;
    angles.z = 0;

    kinematic->ph_Angular_velocity = angles * strength;
}

Vector3f SteeringBehaviour::SetPositionInBounds(Vector3f position, float mapsize)
{
    OPTICK_EVENT();
    return position = {(Mod<float>((position.x), mapsize)), position.y, (Mod<float>((position.z), mapsize))};
}

void SteeringBehaviour::DampenVelocity(cPhysics_Kinematic *kinematic, float strength)
{
    OPTICK_EVENT();
    kinematic->ph_acceleration += -kinematic->ph_velocity.GetNormalized() * strength;
}

void SteeringBehaviour::DampenAngularVelocity(cPhysics_Kinematic *kinematic, float strength)
{
    OPTICK_EVENT();
    kinematic;
    strength;
    throw std::logic_error("The method or operation is not implemented.");
}

void SteeringBehaviour::VelocityMatching(cPhysics_Kinematic *kinematic, Vector3f targetVelocity, float timeToMatch)
{
    OPTICK_EVENT();
    Vector3f acceleration = (targetVelocity - kinematic->ph_velocity) / timeToMatch;
    kinematic->ph_acceleration += acceleration;
}

void SteeringBehaviour::Cohesion(cPhysics_Kinematic *kinematic, Vector3f position,
                                 MultipleTargets_PollingStation *pollingStation, float radius, float strength)
{
    OPTICK_EVENT();
    int count = 0;
    Vector3f CoM = pollingStation->GetCoMWithinCircle(position, radius, count);
    if (count)
    {
        kinematic->ph_acceleration += (CoM - position).GetNormalized() * strength;
    }
}

void SteeringBehaviour::Wander(cPhysics_Kinematic *kinematic, Vector3f forward, float strength)
{
    OPTICK_EVENT();
    // kinematic->ph_Angular_velocity += {0,std::powf(RandomEngine::RandomBinomial(),5)* strength,0};
    Vector3f randomness = {RandomEngine::RandomBinomial(), 0, RandomEngine::RandomBinomial()};
    randomness.Normalize();

    kinematic->ph_acceleration += (forward + randomness * strength);
    // kinematic->ph_acceleration += {
    //	std::powf(RandomEngine::RandomBinomial(),5)* strength,
    //	0,
    //	std::powf(RandomEngine::RandomBinomial(),5)* strength};
}

bool SteeringBehaviour::Arrive(cPhysics_Kinematic *kinematic, Vector3f targetPosition, Vector3f yourPosition,
                               float targetRadius, float slowRadius, float timeToMaxSpeed)
{
    Vector3f targetVelocity;
    Vector3f direction = (targetPosition - yourPosition);
    const float distance = direction.Length();
    direction.Normalize();

    if (distance < targetRadius)
    {
        kinematic->ph_velocity = {0, 0, 0};
        kinematic->ph_acceleration = {0, 0, 0};
        return true;
    }

    if (distance > slowRadius)
    {
        targetVelocity = direction * kinematic->ph_maxSpeed;
    }
    else
    {
        targetVelocity = kinematic->ph_maxSpeed * direction * (distance / slowRadius);
    }

    kinematic->ph_acceleration += (targetVelocity - kinematic->ph_velocity) / timeToMaxSpeed;

    // Implement following moving target with overload of second kinematic of target
    return false;
}

void SteeringBehaviour::Separation(const std::vector<MultipleTargets_PollingStation::DataTuple> &arg,
                                   cPhysics_Kinematic *physicsComponent, const Vector3f &position,
                                   const SY::UUID IgnoreID, SeparationSettings settings)
{
    OPTICK_EVENT();
    for (auto const &i : arg)
    {
        if (i.sourceObject == IgnoreID)
        {
            continue;
        }

        Vector3f direction = (i.positionData - position);
        float distance = direction.Length();

        // Try closest AABB point
        if (auto collider = Scene::ActiveManager().TryGetComponent<Collider>(i.sourceObject))
        {
        }

        if (distance < settings.threshold)
        {
            float strength = std::min(settings.decayCoefficient / (distance * distance), settings.maxAcceleration);

            direction.Normalize();
            physicsComponent->ph_acceleration += strength * -direction;
        }
    }
}
void SteeringBehaviour::Separation(const Vector3f positionToSeparateFrom, cPhysics_Kinematic *physicsComponent,
                                   const Vector3f &position, SeparationSettings settings)
{
    OPTICK_EVENT();
    Vector3f direction = (positionToSeparateFrom - position);
    float distance = direction.Length();

    if (distance < settings.threshold)
    {
        float strength = std::min(settings.decayCoefficient / (distance * distance), settings.maxAcceleration);

        direction.Normalize();
        physicsComponent->ph_acceleration += strength * -direction;
    }
}
// no moving backward just move out from wall like a sane person
void SteeringBehaviour::WallSeparation(const Vector3f positionToSeparateFrom, const Vector3f wallNormal,
                                       cPhysics_Kinematic *physicsComponent, const Vector3f &position,
                                       SeparationSettings settings)
{
    OPTICK_EVENT();
    Vector3f AntiNormal = -wallNormal;

    const Vector3f direction = (positionToSeparateFrom - position);
    const float distance = direction.Length();
    const Vector3f moveDirection =
        (-AntiNormal * direction.Dot(AntiNormal) - AntiNormal * 0.0001f).GetNormalized(); // GetMathed

    if (distance < settings.threshold)
    {
        float strength = std::min(settings.decayCoefficient / (distance * distance), settings.maxAcceleration);
        physicsComponent->ph_acceleration += strength * moveDirection;
    }
}
