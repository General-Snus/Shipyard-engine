#include "Mapper.hpp"
#include "Game\Timer.h"


void InputMapper::AddListener(size_t entity, Keys key, Action action, MapperFunction func)
{
	GetKeyHandler(key).AddListener(entity, action, func);
}

InputKeyHandler& InputMapper::GetKeyHandler(Keys key)
{
	return keyHandlers[int(key)];
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
	state.phase = None;
	state.heldDuration = 0.0f;
}

void InputKeyHandler::Update(InputMapper& ref)
{
	auto keyPressedThisFrame = ref.manager->IsKeyPressed(state.key);
	auto keyHeldThisFrame = ref.manager->IsKeyHeld(state.key);
	auto keyReleasedThisFrame = ref.manager->IsKeyReleased(state.key);

	if (keyPressedThisFrame)
	{
		state.phase = Pressed;
		state.heldDuration = 0;
		state.mouseStartPosition = Input.GetMousePosition();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else if (keyHeldThisFrame)
	{
		state.phase = Held;
		state.heldDuration += TimerInstance.getDeltaTime();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else if (keyReleasedThisFrame)
	{
		state.phase = Released;
		state.heldDuration += TimerInstance.getDeltaTime();
		state.mouseCurrentPosition = Input.GetMousePosition();
	}
	else
	{
		state.phase = None;
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

void InputKeyHandler::AddListener(size_t entity, Action action, MapperFunction func)
{
	RemoveListener(entity, action);

	InputListener  listener;

	listener.action = action;
	listener.id = entity;
	listener.onInputFunc = func;

	listeners.emplace_back(listener);
}

void InputKeyHandler::RemoveListener(size_t entity, Action action)
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
