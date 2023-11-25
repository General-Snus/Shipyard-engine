#pragma once
class cPhysics_Kinematic : public Component
{
public:
	void Init() override;
	void Update() override;
	void Render() override;

	void SetVelocity(const Vector3f& aVelocity);
	void SetAcceleration(const Vector3f& anAcceleration);
	void AddVelocity(const Vector3f& aVelocity);
	void AddAcceleration(const Vector3f& anAcceleration);
private:
	Vector3f ph_velocity;
	Vector3f ph_acceleration;
};

