#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include "Input.hpp"
#include "EnumKeys.h"

#define Mapper ServiceLocator::Instance().GetService<InputMapper>()
class InputMapper;

enum Action {
	UIClick,
	MoveCharacter
};

enum Phase {
	None,
	Pressed,
	Held,
	Released
};
struct InputState
{
public:
	Keys key;
	Phase phase;
	float heldDuration;

	std::optional<Vector2f> mouseStartPosition;
	std::optional<Vector2f> mouseCurrentPosition;
};

enum InputResponse { claimInput, undecided, irrelevant };

struct InputContext
{
public:
	size_t id;
	Action action;
	InputState state;
};

using MapperFunction = std::function<InputResponse(InputContext)>;

struct InputListener {
public:
	size_t id;
	Action action;
	MapperFunction onInputFunc;
};

class InputKeyHandler {
public:
	InputKeyHandler(Keys key);
	void Update(InputMapper& ref);

	void AddListener(size_t entity, Action action,/*Potentiall string for debugging*/ MapperFunction func);
	void RemoveListener(size_t entity, Action action);

	InputState state;
	std::vector<InputListener> listeners;

};
class InputMapper : public Singleton
{
public:
	friend class InputKeyHandler;
	InputMapper() = default;
	void         Update();

	void AddListener(size_t entity, Keys key, Action action,/*Potentiall string for debugging*/ MapperFunction func);
private:
	InputKeyHandler& GetKeyHandler(Keys  key);
	InputManager* manager;
	std::array<InputKeyHandler, static_cast<size_t>(Keys::COUNT)> keyHandlers;
};
