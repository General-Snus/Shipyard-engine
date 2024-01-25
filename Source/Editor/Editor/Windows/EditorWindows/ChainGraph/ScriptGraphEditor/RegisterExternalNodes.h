#pragma once 
#include <MuninScriptGraph.h>
// Example file for how to register nodes in another project, i.e. not inside MuninGraph.lib.
// To get all the nodes inside MuninGraph in your project to auto-register you need to link
// with /WHOLEARCHIVE:MuninGraph.lib or VS will optimize the auto reg away. Similarly if you
// have this file in a lib of your own you need to /WHOLEARCHIVE that file too. If this lives
// in an EXE project then it'll work as intended on its own. 

BeginScriptGraphNode(MVNode_MakeVector)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Make Vector"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_BreakVector)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Break Vector"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};




BeginScriptGraphNode(MVNode_Branch)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Branch"; }
	size_t DoOperation() override;
};

BeginScriptGraphNode(MVNode_ForLoop)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "For"; }
	size_t DoOperation() override;
};

BeginScriptGraphNode(MVNode_GetGameObject)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Get GameObject"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};
BeginScriptGraphNode(MVNode_GetTransformPosition)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Get Transform Position"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_SetTransformPosition)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Set Transform Position"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_SetTransformScale)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Set Transform Scale"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_SetTransformRotation)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Set Transform Rotation"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_GetCursorPosition)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Get Mouse cursor position"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};

BeginScriptGraphNode(MVNode_Distance)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Distance of a vector"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};
BeginScriptGraphNode(MVNode_VectorMinus)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Difference between two vectors"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};
BeginScriptGraphNode(MVNode_ScreenSpacePosition)
{
public:
	void Init() override;
	std::string GetNodeTitle() const override { return "Distance between two vectors"; }
	size_t DoOperation() override;
	bool IsSimpleNode() const override { return false; }
};