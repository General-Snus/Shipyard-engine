#pragma once

#include "../../ScriptGraphNode.h" 

BeginScriptGraphNode(SGNode_MathAdd)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Add"; }
	std::string GetDescription() const override { return "A + B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};


BeginScriptGraphNode(SGNode_MathSubtract)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Subtract"; }
	std::string GetDescription() const override { return "A - B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathMultiply)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Multiply"; }
	std::string GetDescription() const override { return "A * B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathDivide)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Divide"; }
	std::string GetDescription() const override { return "A / B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathPower)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Power"; }
	std::string GetDescription() const override { return "A ^ B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathLogarithm)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Logarithm"; }
	std::string GetDescription() const override { return "Log_B(A)"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathAbsolute)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Absolute"; }
	std::string GetDescription() const override { return "|A|"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathMinimum)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Min"; }
	std::string GetDescription() const override { return "min(A,B)"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathMaximum)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Max"; }
	std::string GetDescription() const override { return "max(A,B)|"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathClamp)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Clamp"; }
	std::string GetDescription() const override { return "Clamp(V,A,B)"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};


BeginScriptGraphNode(SGNode_MathLessThan)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Less Than"; }
	std::string GetDescription() const override { return "A < B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(SGNode_MathGreaterThan)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Greater Than"; }
	std::string GetDescription() const override { return "A > B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};


BeginScriptGraphNode(SGNode_MathSign)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Sign"; }
	std::string GetDescription() const override { return "(+-)1* B"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};


BeginScriptGraphNode(SGNode_MathModulus)
{
public:

	void Init() override;
	std::string GetNodeTitle() const override { return "Modulus"; }
	std::string GetDescription() const override { return "Non truncating modulus"; }
	std::string GetNodeCategory() const override { return "Math"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};