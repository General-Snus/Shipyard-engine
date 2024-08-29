#pragma once
#include <Tools/Logging/Logging.h>
#include <assert.h>
#include <cassert>
#include <vector>

class BaseCommand
{
  public:
    virtual void Do() = 0;
    virtual void Undo() = 0;

    // Try merge with previous command when added, if true adding is redundant
    virtual bool Merge(std::shared_ptr<BaseCommand> &ptr)
    {
        return ptr && !ptr->m_MergeBlocker;
    };
    virtual std::string GetDescription() const
    {
        return Description;
    };
    // Set this to true when you have identical command chains that you want to separate and avoid merging down
    void SetMergeBlocker(bool arg = true);

  protected:
    bool m_MergeBlocker = false;
    std::string Description = "Unknown command";
};

using CommandPacket = std::vector<std::shared_ptr<BaseCommand>>;
class CommandBuffer
{
  public:
    static CommandBuffer &MainEditorCommandBuffer();

    template <typename CommandClass, typename... Args> inline void AddCommand(Args... arguments)
    {
        auto ptr = std::make_shared<CommandClass>(arguments...);

        if (commandList.size() > cursor && ptr->Merge(commandList[cursor].back()))
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

    const std::vector<CommandPacket> &GetCommandList();
    ;

    // assumed complete command
    inline void AddCommand(const CommandPacket &ptr)
    {
        if (ptr.empty() || commandList.size() > cursor && ptr.front()->Merge(commandList[cursor].back()))
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
    inline void AddCommand(std::shared_ptr<BaseCommand> ptr)
    {
        assert(ptr != nullptr);

        if (commandList.size() > cursor && ptr->Merge(commandList[cursor].back()))
        {
            return;
        }

        if (!commandList.empty())
        {
            commandList.erase(commandList.begin() + (cursor + 1), commandList.begin() + commandList.size());
        }
        commandList.push_back({ptr});
        cursor = static_cast<int>(commandList.size()) - 1;
    };

    template <typename T> inline T *GetLastCommand()
    {
        const auto out = dynamic_cast<T>(commandList.back());
        assert(out != nullptr);
        return out;
    };

    inline BaseCommand *GetLastCommand()
    {
        if (commandList.empty() || commandList.back().empty())
        {
            return nullptr;
        }
        return commandList.back().back().get();
    };

    void Undo()
    {
        if (!commandList.empty() && cursor >= 0)
        {
            Logger::Log(std::format("Undoing command at {}", cursor));
            for (auto &command : commandList[cursor])
            {
                command->Undo();
            }
            cursor--;
        }
    };

    void Redo()
    {
        if (commandList.size() > cursor + 1)
        {
            Logger::Log(std::format("Redoing command at {}", cursor));
            for (auto &command : commandList[cursor + 1])
            {
                command->Do();
            }

            cursor++;
        }
    };

    int GetCursor() const
    {
        return cursor;
    };

  private:
    std::vector<CommandPacket> commandList;
    int cursor = 0;
};
