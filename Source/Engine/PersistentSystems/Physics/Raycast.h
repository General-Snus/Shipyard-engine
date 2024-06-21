#pragma once
#include <Engine/AssetManager/ComponentSystem/GameObject.h>


namespace Physics {
	struct RaycastHit
	{
		GameObject objectHit;
		Vector3f point;
		Vector3f normal;
	};

	bool Raycast(Vector3f origin, Vector3f direction, RaycastHit& outHitInfo, GameObject filter, Layer layerMask = Layer::AllLayers, float maxDistance = FLT_MAX);
	bool Raycast(Vector3f origin, Vector3f direction, RaycastHit& outHitInfo, Layer layerMask = Layer::AllLayers, float maxDistance = FLT_MAX);
	bool Raycast(Vector3f origin, Vector3f direction, Layer layerMask = Layer::AllLayers, float maxDistance = FLT_MAX);
}

