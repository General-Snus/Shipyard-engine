#include "AssetManager.pch.h"
#include "../Component.h"
#include <cmath>

void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1),0);
}
