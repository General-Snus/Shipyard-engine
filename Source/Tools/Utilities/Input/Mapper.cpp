#include "Mapper.hpp"
#include "Game\Timer.h"
#include <numeric>
#include "Tools\Utilities\TemplateHelpers.h"

void InputMapper::AddListener(uint32_t entity, Keys key, Action action, MapperFunction func)
{
	GetKeyHandler(key).AddListener(entity, action, func);
}

InputKeyHandler& InputMapper::GetKeyHandler(Keys key)
{
	return keyHandlers[int(key)];
}

InputMapper::InputMapper() :manager(&Input)
{
	//Template flexing here
	auto iota = std::views::iota(0, Cast<int>(Keys::COUNT));
	auto castedView = ViewCast<Keys>();

	for (auto key : iota | castedView)
	{
		keyHandlers[to_index(key)] = InputKeyHandler(key);
	}
}

void InputMapper::Update()
{
	for (auto& i : keyHandlers)
	{
		i.Update(*this);
	}
}

InputKeyHandler::InputKeyHandler(Keys key)
{
	state.key = key;
	state.phase = Phase::None;
	state.heldDuration = 0.0f;
}

void InputKeyHandler::Update(InputMapper& ref)
{
	auto keyPressedThisFrame = ref.manager->IsKeyPressed(state.key);
	auto keyHeldThisFrame = ref.manager->IsKeyHeld(state.key);
	auto keyReleasedThisFrame = ref.manager->IsKeyReleased(state.key);

	if (keyPressedThisFrame)
	{
		state.phase = Phase::Pressed;
		state.heldDuration = 0;
		state.mouseStartPosition = Input.GetMousePosition();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else if (keyHeldThisFrame)
	{
		state.phase = Phase::Held;
		state.heldDuration += TimerInstance.getDeltaTime();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else if (keyReleasedThisFrame)
	{
		state.phase = Phase::Released;
		state.heldDuration += TimerInstance.getDeltaTime();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else
	{
		state.phase = Phase::None;
		return;
	}

	for (const auto& callers : listeners)
	{
		InputContext context;
		context.action = callers.action;
		context.id = callers.id;
		context.state = state;

		auto reponse = callers.onInputFunc(context);

		switch (reponse)
		{
		case InputResponse::claimInput:
			return;
		case InputResponse::undecided:
			break;
		case InputResponse::irrelevant:
			break;
		default:
			throw new std::exception("Fucked in the mapper");
		}
	}

}

void InputKeyHandler::AddListener(uint32_t entity, Action action, MapperFunction func)
{
	RemoveListener(entity, action);

	InputListener  listener;

	listener.action = action;
	listener.id = entity;
	listener.onInputFunc = func;

	listeners.emplace_back(listener);
}

void InputKeyHandler::RemoveListener(uint32_t entity, Action action)
{

	for (size_t i = 0; i < listeners.size(); i++)
	{
		const auto& listener = listeners[i];
		if (listener.action == action && listener.id == entity)
		{
			listeners[i] = listeners.back();
			listeners.pop_back();
		}
	}
}
