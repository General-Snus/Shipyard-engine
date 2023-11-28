#pragma once

class WanderController : public Controller
{
public:
	explicit WanderController(GameObject componentCheck);
	bool Update(GameObject input) override;
	void Recieve(const AIEvent& aEvent) override;
private:

};

