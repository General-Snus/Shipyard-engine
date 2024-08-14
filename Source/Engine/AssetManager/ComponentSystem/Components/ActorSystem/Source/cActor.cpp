#include "Engine/AssetManager/AssetManager.pch.h"

#include "../cActor.h"

cActor::cActor(const SY::UUID anOwnerId, GameObjectManager *aManager) : Component(anOwnerId, aManager)
{
}

void cActor::Init()
{
}

void cActor::Update()
{
    OPTICK_EVENT();
    if (auto *transform = TryGetComponent<Transform>()) // Actors will was accepted, do checks around here if allowed to
                                                        // move external forces such as transform lock
    {
        if (controller)
        {
            controller->Update(GetGameObject()); // Im giving the controller all the rights over the object here
        }
    }
}
bool cActor::InspectorView()
{
    if (!Component::InspectorView())
    {
        return false;
    }
    Reflect<cActor>();

    std::string controllerType = "No controller";
    if (controller)
    {
        controllerType = "Unknown type";
        {
            controllerType = controller->GetTypeInfo().Name();
        }
    }
    ImGui::Text(std::format("Controller type: {}", controllerType).c_str());

    if (controller)
    {
        controller->InspectorView();
    }
    return true;
}

void cActor::SetController(Controller *aController)
{
    controller = aController;
    controller->ComponentRequirement(GetGameObject());
}
