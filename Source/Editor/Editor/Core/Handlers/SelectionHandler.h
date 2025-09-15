#pragma once
#include "Collection.h"
#include <vector>
#include "Engine\AssetManager\ComponentSystem\GameObject.h"
#include <algorithm>

class SelectionHandler : Handler
{
	friend class HandlerCollection;
public:

	std::vector<GameObject>& GetSelectedGameObjects()
	{
		// Todo ensure saftey from other steps
		const auto& invalidArray =
			std::ranges::remove_if(m_SelectedGameObjects, [](GameObject obj) { return !obj.IsValid(); });
		m_SelectedGameObjects.erase(invalidArray.begin(), invalidArray.end());

		return m_SelectedGameObjects;
	}

private:
	std::vector<GameObject> m_SelectedGameObjects;
	std::vector<GameObject> copiedObjects;
	void Update();
};
