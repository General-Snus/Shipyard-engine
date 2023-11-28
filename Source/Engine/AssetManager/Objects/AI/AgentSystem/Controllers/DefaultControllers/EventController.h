#pragma once  
class EventController : public Controller
{
public: 
	explicit EventController();
	bool Update(GameObject  aPosition) override;
	void Recieve(const AIEvent& aEvent) override;
private:
	bool pathToTarget;
	Vector3f end;
	NavMesh::Path myPath;
};