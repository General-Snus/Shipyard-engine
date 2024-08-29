#pragma once
#include <unordered_set>

class CollisionChecks
{
  public:
    static void CheckColliders();
    static void RemoveCollisions(const SY::UUID &anOwnerID);
    static inline std::unordered_map<SY::UUID, std::unordered_map<SY::UUID, bool>> activeCollisions;
};
