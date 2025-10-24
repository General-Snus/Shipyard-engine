#pragma once
#include <Tools/Utilities/System/ServiceLocator.h>
#include "Input.hpp"
#include "EnumKeys.h"
#include <array>

#define Mapper ServiceLocator::Instance().GetService<InputMapper>()
class InputMapper;

enum class Action {
	None = 0,
	UIClick,
	MoveCharacter
};

enum class Phase {
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
	uint32_t id;
	Action action;
	InputState state;
};

using MapperFunction = std::function<InputResponse(InputContext)>;

struct InputListener {
public:
	uint32_t id;
	Action action;
	MapperFunction onInputFunc;
};

class InputKeyHandler {
public:
	InputKeyHandler() = default;
	InputKeyHandler(Keys key);
	void Update(InputMapper& ref);

	void AddListener(uint32_t entity, Action action,/*Potentiall string for debugging*/ MapperFunction func);
	void RemoveListener(uint32_t entity, Action action);

	InputState state;
	std::vector<InputListener> listeners;
};

class InputMapper : public Singleton
{
public:
	friend class InputKeyHandler;
	InputMapper();
	void         Update();

	void AddListener(uint32_t entity, Keys key, Action action,/*Potentiall string for debugging*/ MapperFunction func);
private:
	InputKeyHandler& GetKeyHandler(Keys  key);
	InputManager* manager;
	std::array<InputKeyHandler, to_index(Keys::COUNT)> keyHandlers{};
};
