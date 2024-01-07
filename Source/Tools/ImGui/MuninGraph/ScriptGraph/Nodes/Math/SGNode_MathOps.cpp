#include "MuninGraph.pch.h"
#include "SGNode_MathOps.h"
#include <Math.hpp>

void SGNode_MathAdd::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathAdd::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA + inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathSubtract::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathSubtract::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA - inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathMultiply::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathMultiply::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA * inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathDivide::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathDivide::DoOperation()
{
	float inA = 0;
	float inB = 1;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA / inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathPower::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("Base",PinDirection::Input);
	CreateDataPin<float>("Exponent",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathPower::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("Base",inA) && GetPinData("Exponent",inB))
	{
		const float result = std::powf(inA,inB);
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathLogarithm::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("Value",PinDirection::Input);
	CreateDataPin<float>("Base",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathLogarithm::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("Value",inA) && GetPinData("Base",inB))
	{
		if(0.00001f > inB)
		{
			const float result = std::log2f(inA) / std::log2f(inB);
			SetPinData("Result",result);
		}
		else
		{
			SetPinData("Result",0.f);
		}
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathAbsolute::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("Value",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathAbsolute::DoOperation()
{
	float inA = 0;
	if(GetPinData("Value",inA))
	{
		const float result = std::fabsf(inA);
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathMinimum::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathMinimum::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA < inB ? inA : inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathMaximum::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathMaximum::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const float result = inA > inB ? inA : inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathClamp::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("Value",PinDirection::Input);
	CreateDataPin<float>("Upper",PinDirection::Input);
	CreateDataPin<float>("Lower",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathClamp::DoOperation()
{
	float inA = 0;
	float inB = 0;
	float inC = 0;
	if(GetPinData("Value",inA) && GetPinData("Upper",inB) && GetPinData("Lower",inC))
	{
		if(inC < inB) // avoid std clamp throwing a tantrum
		{
			SetPinData("Result",0.f);
			return ExitWithError("The lower value is greater than the upper value");
		}
		else
		{
			const float result = std::clamp<float>(inA,inC,inB);
			SetPinData("Result",result);
		}
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathLessThan::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<bool>("Result",PinDirection::Output);
}

size_t SGNode_MathLessThan::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const bool result = inA < inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}


void SGNode_MathGreaterThan::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("B",PinDirection::Input);
	CreateDataPin<bool>("Result",PinDirection::Output);
}

size_t SGNode_MathGreaterThan::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("A",inA) && GetPinData("B",inB))
	{
		const bool result = inA > inB;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}

void SGNode_MathSign::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("A",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathSign::DoOperation()
{
	float inA = 0;
	if(GetPinData("A",inA))
	{
		const float result = std::signbit(inA) ? -1.f : 1.f;
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}



void SGNode_MathModulus::Init()
{
	CreateExecPin("In",PinDirection::Input,true);
	CreateExecPin("Out",PinDirection::Output,true);
	CreateDataPin<float>("Value",PinDirection::Input);
	CreateDataPin<float>("Remainder",PinDirection::Input);
	CreateDataPin<float>("Result",PinDirection::Output);
}

size_t SGNode_MathModulus::DoOperation()
{
	float inA = 0;
	float inB = 0;
	if(GetPinData("Value",inA) && GetPinData("Remainder",inB))
	{
		const float result = Mod<float>(inA,inB);
		SetPinData("Result",result);
		return ExitViaPin("Out");
	}
	return 0;
}