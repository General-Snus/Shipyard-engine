#include "AssetManager.pch.h"

#include <cmath>
#include "../Component.h"


void Component::Abandon()
{
	IsInherited = std::max((IsInherited - 1),0);
}

__forceinline void Component::InspectorView()
{


}
