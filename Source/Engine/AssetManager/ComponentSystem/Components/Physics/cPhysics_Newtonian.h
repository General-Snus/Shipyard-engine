#pragma once
class cPhysics_Newtonian : public Component
{
public:
	cPhysics_Newtonian(const SY::UUID anOwnerID);
	void Init() override;
	void Update() override;
	void Render() override;
private:
	Vector3f ph_force;
	double ph_mass;
};

