#pragma once 

class PollingStation
{
public:
	PollingStation() = default;

	inline std::string GetGivenName() const
	{
		return givenName;
	};
	inline void SetGivenName(const std::string& aGivenName)
	{
		givenName = aGivenName;
	};

protected:
	std::string givenName;
};