#pragma once
#include <assert.h>
#include <cassert>
#include <vector>
#include <Tools/Logging/Logging.h>

class BaseCommand
{
public:
	virtual void commandRedo() = 0;
	virtual void commandUndo() = 0;

	// Try merge with previous command when added, if true adding is redundant
	virtual bool merge(std::shared_ptr<BaseCommand>& ptr)
	{
		return ptr && !ptr->mergeBlocker;
	}

	virtual std::string getDescription() const
	{
		return description;
	}

	// Set this to true when you have identical command chains that you want to separate and avoid merging down
	virtual void setMergeBlocker(bool arg = true)
	{
		mergeBlocker = arg;
	}

protected:
	bool        mergeBlocker = false;
	std::string description = "Unknown command";
};


using CommandPacket = std::vector<std::shared_ptr<BaseCommand>>;

class CommandBuffer
{
public:
	static CommandBuffer& mainEditorCommandBuffer()
	{
		static CommandBuffer instance;
		return instance;
	}

	template <typename CommandClass, typename... Args>
	void addCommand(Args... arguments)
	{
		auto ptr = std::make_shared<CommandClass>(arguments...);

		if (commandList.size() > cursor && ptr->merge(commandList[cursor].back()))
		{
			return;
		}

		if (!commandList.empty())
		{
			commandList.erase(commandList.begin() + (cursor + 1), commandList.end());
		}

		commandList.push_back({ptr});
		cursor = static_cast<int>(commandList.size()) - 1;
	}

	const std::vector<CommandPacket>& getCommandList()
	{
		return commandList;
	}

	// assumed complete command 
	void addCommand(const CommandPacket& ptr)
	{
		if (ptr.empty() || commandList.size() > cursor && ptr.front()->merge(commandList[cursor].back()))
		{
			return;
		}

		if (!commandList.empty())
		{
			commandList.erase(commandList.begin() + (cursor + 1), commandList.begin() + commandList.size());
		}
		commandList.push_back(ptr);
		cursor = static_cast<int>(commandList.size()) - 1;
	}

	// assumed complete command
	void addCommand(std::shared_ptr<BaseCommand> ptr)
	{
		assert(ptr != nullptr);

		if (commandList.size() > cursor && ptr->merge(commandList[cursor].back()))
		{
			return;
		}

		if (!commandList.empty())
		{
			commandList.erase(commandList.begin() + (cursor + 1), commandList.begin() + commandList.size());
		}
		commandList.push_back({ptr});
		cursor = static_cast<int>(commandList.size()) - 1;
	}

	template <typename T>
	T* getLastCommand()
	{
		const auto out = dynamic_cast<T>(commandList.back());
		assert(out != nullptr);
		return out;
	}


	BaseCommand* getLastCommand() const
	{
		if (commandList.empty() || commandList.back().empty())
		{
			return nullptr;
		}
		return commandList.back().back().get();
	}


	void undo()
	{
		if (!commandList.empty() && cursor >= 0)
		{
			LOGGER.Log(std::format("Undoing command at {}", cursor));
			for (const auto& command : commandList[cursor])
			{
				command->commandUndo();
			}
			cursor--;
		}
	}

	void redo()
	{
		if (commandList.size() > cursor + 1)
		{
			LOGGER.Log(std::format("Redoing command at {}", cursor));
			for (const auto& command : commandList[cursor + 1])
			{
				command->commandRedo();
			}

			cursor++;
		}
	}

	size_t getCursor() const
	{
		return cursor;
	}

private:
	std::vector<CommandPacket> commandList;
	size_t                     cursor = 0;
};
