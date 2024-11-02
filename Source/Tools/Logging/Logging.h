#pragma once
#include <format>
#include <mutex>
#include <source_location>
#include <string>
#include <vector>

#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include <Tools/Utilities/Color.h>
#include <Tools/Utilities/TemplateHelpers.h>

#define Logger ServiceLocator::Instance().GetService<LoggerService>()
class LoggerService : public Singleton
{
  public:
	LoggerService() = default;
	enum class LogType : int32_t
	{
		none = 0,
		message = 1 << 0,
		warning = 1 << 1,
		error = 1 << 2,
		critical = 1 << 3,
		success = 1 << 4,
		All = INT32_MAX
	};

  private:
	struct LogMsg
	{
		LogType messageType;
		std::string message;
	};
	struct logBuffer
	{
		std::vector<LogMsg> LoggedMessages;
		unsigned messagesCount{};
		unsigned warnCount{};
		unsigned errCount{};
		unsigned criticalCount{};
		unsigned successCount{};

		void Add(LogMsg msg)
		{
			switch (msg.messageType)
			{
				using enum LoggerService::LogType;
			case message:
				messagesCount++;
				break;
			case warning:
				warnCount++;
				break;
			case error:
				errCount++;
				break;
			case critical:
				criticalCount++;
				break;
			case success:
				successCount++;
				break;
			default:
				break;
			}
			LoggedMessages.emplace_back(msg);
		}
	};

  public:
	logBuffer m_Buffer;
	bool Create();
	void SetConsoleHandle(void *aHandle);
	void SetPrintToVSOutput(bool bNewValue);
	void Log(const char *aString, const std::source_location &location = std::source_location::current());
	Color GetColor(LogType type);

	template <typename T>
	void Log(const T &aString, const std::source_location &location = std::source_location::current())
	{
		Log(std::to_string(aString));
	}

	void Log(const std::string &aString, const std::source_location &location = std::source_location::current());
	void Warn(const std::string &aString, const std::source_location &location = std::source_location::current());

	void Err(const std::string &aString, const std::source_location &location = std::source_location::current());
	void Succ(const std::string &aString, const std::source_location &location = std::source_location::current());
	void Critical(const std::exception &anException, unsigned aLevel = 0,
				  const std::source_location &location = std::source_location::current());
	void Critical(const std::string &anExceptionText, unsigned aLevel = 0,
				  const std::source_location &location = std::source_location::current());
	void NewLine();
	void *GetHandle() const
	{
		return myHandle;
	}

  private:
	void *myHandle = 0;
	bool shouldPrintToOutput = false;
	bool isInitialized = false;
	std::string myNamespace;
	std::mutex readyToWrite;
	[[nodiscard]] std::string Timestamp();
};

ENABLE_ENUM_BITWISE_OPERATORS(LoggerService::LogType)
