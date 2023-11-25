#pragma once
class cPhysics_Newtonian : public Component
{
public:
	void Init() override;
	void Update() override;
	void Render() override;
private:
	Vector3f ph_force;
	double ph_mass;
};

