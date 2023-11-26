#pragma once
class cPhysics_Kinematic : public Component
{
public:
	cPhysics_Kinematic(const SY::UUID anOwnerID);
	void Init() override;
	void Update() override;
	void Render() override; 

	Vector3f ph_velocity;
	Vector3f ph_acceleration;
	Vector3f ph_Angular_acceleration;
	Vector3f ph_Angular_velocity;
	bool bindAngleToVelocity;
	bool localVelocity;
	float ph_maxSpeed;
private:

};

