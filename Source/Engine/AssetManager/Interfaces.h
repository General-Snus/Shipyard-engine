#pragma once


class AvailableInInspector
{
public:
	virtual ~AvailableInInspector() = default;
	virtual bool InspectorView() = 0;
	
private:
	bool Inspect(this auto&& self) { return self.InspectorView(); }
};
