#pragma once
#include "PollingStations/PollingStation.h"
#include <Tools/Utilities/System/SingletonTemplate.h>

#include <Tools/Utilities/System/ServiceLocator.h>
#define AIPollingManagerInstance ServiceLocator::Instance().GetService<AIPollingManager>()
class AIPollingManager : public Singleton
{
  public:
    void AddStation(const std::string &aGivenName, std::shared_ptr<PollingStation> aNewStation);
    std::shared_ptr<PollingStation> GetStation(const std::string &aName);

    template <typename T> std::shared_ptr<T> GetStation(const std::string &aName);

  private:
    std::unordered_map<std::string, std::shared_ptr<PollingStation>> myPollingStations;
};
