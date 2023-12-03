#pragma once

class cPhysics_Kinematic;
class SteeringBehaviour
{
public:
	//Strength of x means it will reach the desired angle in 1/x second
	static void LookAt(cPhysics_Kinematic* kinematic,Vector3f TargetDirection,Vector3f forward,float strength = 1.0f);
	static Vector3f SetPositionInBounds(Vector3f position,float mapsize);
	static void DampenVelocity(cPhysics_Kinematic* kinematic,float strength = 0.5f);
	static void DampenAngularVelocity(cPhysics_Kinematic* kinematic,float strength = 0.5f);
	static void VelocityMatching(cPhysics_Kinematic* kinematic,Vector3f targetVelocity,float timeToMatch = 1.0f,float MaxAcceleration = 100.0f);
	static void Cohesion(cPhysics_Kinematic* kinematic,Vector3f position,MultipleTargets_PollingStation* pollingStation,float radius = 10.0f,float strength = 1.0f);


	struct SeparationSettings
	{
		float threshold = 5.0f;
		float decayCoefficient = 5.0f;
		float maxAcceleration = 100.0f;
	};
	static void Separation(const std::vector<MultipleTargets_PollingStation::DataTuple>& arg,cPhysics_Kinematic* physicsComponent,const Vector3f& position,const SY::UUID IgnoreID,SeparationSettings settings = {});

};

