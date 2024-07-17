#pragma once
#include <vector>
#include <assert.h> 

class BaseCommand
{
public:
	virtual void Do() = 0;
	virtual void Undo() = 0; 

	//Try merge with previous command when added, if true adding is redundant
	virtual bool Merge(BaseCommand* ptr) = 0;
};


class CommandBuffer
{
public:
	static 	CommandBuffer& MainEditorCommandBuffer();

	template<typename CommandClass, typename ...Args>
	void AddCommand(Args ... arguments)
	{
		BaseCommand* ptr = CommandClass(arguments ...);
		ptr->Do();

		commandList.erase(commandList.begin() + cursor + 1, commandList.end());
		commandList.push_back(ptr);
		cursor = commandList.size() - 1;
	};

	// assumed complete command
	void AddCommand(BaseCommand* ptr)
	{
		assert(ptr != nullptr);

		if (ptr->Merge(commandList[cursor]))
		{
			return;
		}

		commandList.erase(commandList.begin() + cursor + 1, commandList.end());
		commandList.push_back(ptr);
		cursor = static_cast<int>(commandList.size()) - 1;
	};


	template<typename T>
	T* GetLastCommand()
	{
		const auto out = dynamic_cast<T>(commandList.back());
		assert(out != nullptr);
		return out;
	};

	void Undo()
	{
		if (!commandList.empty() && cursor > 0)
		{
			commandList[cursor]->Undo();
			cursor--;
		}
	};

	void Redo()
	{
		if (!commandList.empty())
		{
			commandList[cursor]->Do();
			cursor++;
		}
	};

private:
	std::vector<BaseCommand*> commandList;
	int cursor = 0;
};

