#pragma once
class cPhysics_Newtonian : public Component
{
public:
	MYLIB_REFLECTABLE();
    cPhysics_Newtonian(const SY::UUID anOwnerId, GameObjectManager *aManager);
    void Init() override;
    void Update() override; 
    bool InspectorView() override;
    Vector3f ph_force;
    double ph_mass;
};

REFL_AUTO(type(cPhysics_Newtonian), field(ph_force), field(ph_mass))
