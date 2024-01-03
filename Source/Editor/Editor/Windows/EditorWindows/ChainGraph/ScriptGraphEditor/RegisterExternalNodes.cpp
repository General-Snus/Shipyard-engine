#include "PersistentSystems.pch.h"
#include "RegisterExternalNodes.h"
#include <Tools/Utilities/LinearAlgebra/Vectors.hpp>

void MVNode_TestNode::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Out", PinDirection::Output, true);
	CreateDataPin<float>("Value", PinDirection::Input);
}

size_t MVNode_TestNode::DoOperation()
{
	return ExitViaPin("Out");
}

void MVNode_TestExecless::Init()
{
	CreateDataPin<bool>("Test", PinDirection::Input);
	CreateDataPin<bool>("Value", PinDirection::Output);
}

size_t MVNode_TestExecless::DoOperation()
{
	bool b = false;
	GetPinData("Test", b);
	SetPinData("Value", b);
	return Exit();
}

void MVNode_TestMultiInput::Init()
{
	CreateDataPin<float>("A", PinDirection::Input);
	CreateDataPin<float>("B", PinDirection::Input);
	CreateDataPin<float>("C", PinDirection::Input);
	CreateDataPin<float>("Result", PinDirection::Output);
}

size_t MVNode_TestMultiInput::DoOperation()
{
	return Exit();
}

bool MVNode_TestMultiInput::IsSimpleNode() const
{
	return false;
}

void MVNode_MakeVector::Init()
{
	CreateDataPin<float>("X", PinDirection::Input);
	CreateDataPin<float>("Y", PinDirection::Input);
	CreateDataPin<float>("Z", PinDirection::Input);
	CreateDataPin<Vector3f>("Result", PinDirection::Output);
}

size_t MVNode_MakeVector::DoOperation()
{
	float X = 0, Y = 0, Z = 0;
	if(GetPinData("X", X) && GetPinData("Y", Y) && GetPinData("Z", Z))
	{
		Vector3f result(X, Y, Z);
		SetPinData("Result", result);
		return Exit();
	}

	return 0;
}

void MVNode_BreakVector::Init()
{
	CreateDataPin<Vector3f>("Vector", PinDirection::Input);
	CreateDataPin<float>("X", PinDirection::Output);
	CreateDataPin<float>("Y", PinDirection::Output);
	CreateDataPin<float>("Z", PinDirection::Output);
}

size_t MVNode_BreakVector::DoOperation()
{
	Vector3f vector;
	if(GetPinData("Vector", vector))
	{
		SetPinData("X", vector.x);
		SetPinData("Y", vector.y);
		SetPinData("Z", vector.z);
		return Exit();
	}

	return 0;
}

void MVNode_Branch::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("True", PinDirection::Output);
	CreateExecPin("False", PinDirection::Output);

	CreateDataPin<bool>("Condition", PinDirection::Input);
}

size_t MVNode_Branch::DoOperation()
{
	bool condition = false;
	if(GetPinData("Condition", condition))
	{
		return ExitViaPin(condition ? "True" : "False");
	}

	return 0;
}

void MVNode_ForLoop::Init()
{
	CreateExecPin("In", PinDirection::Input, true);
	CreateExecPin("Loop", PinDirection::Output);

	CreateDataPin<int>("Start", PinDirection::Input);
	CreateDataPin<int>("End", PinDirection::Input);
	CreateDataPin<int>("Index", PinDirection::Output);
	CreateExecPin("Completed", PinDirection::Output);
}

size_t MVNode_ForLoop::DoOperation()
{
	return 0;
}
