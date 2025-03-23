#pragma once
#include <format>
#include <mutex>
#include <source_location>
#include <stacktrace>
#include <string> 

#include <Tools/Utilities/Color.h>
#include <Tools/Utilities/TemplateHelpers.h>
#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include <deque>

#define LOGGER ServiceLocator::Instance().GetService<LoggerService>()

class LoggerService : public Singleton
{
public:
	LoggerService();

	enum class LogType : int32_t
	{
		none     = 0,
		message  = 1 << 0,
		warning  = 1 << 1,
		error    = 1 << 2,
		critical = 1 << 3,
		success  = 1 << 4,
		All      = INT32_MAX
	};

	struct LogMsg
	{
		LogType         messageType;
		std::string     message;
		std::stacktrace trace;
	};

private:
	static constexpr int maxsize = 10'000;
	struct logBuffer
	{
		std::deque<LogMsg> LoggedMessages;
		unsigned            messagesCount{};
		unsigned            warnCount{};
		unsigned            errCount{};
		unsigned            criticalCount{};
		unsigned            successCount{};

		void Add(const LogMsg& msg)
		{
			switch (msg.messageType)
			{
				using enum LogType;
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
			if(LoggedMessages.size() > maxsize)
			{
				LoggedMessages.pop_front();
			}

			LoggedMessages.emplace_back(msg);
		}
	};

public:
	logBuffer m_Buffer;
	bool      Create();
	void      SetConsoleHandle(void* aHandle);
	void      SetPrintToVSOutput(bool bNewValue);
	void      Log(const char*            aString, bool withNotice = false,
	         const std::source_location& location = std::source_location::current());
	static Color GetColor(LogType type);

	

	void Log(const std::string&          aString, bool withNotice = false,
	         const std::source_location& location = std::source_location::current());

	void Warn(const std::string&          aString, bool withNotice = false,
	          const std::source_location& location = std::source_location::current());


	void Err(const std::string&          aString, bool withNotice = false,
	         const std::source_location& location = std::source_location::current());
 
	void ErrTrace(const std::string&          aString, bool withNotice = false,
	              const std::stacktrace&      trace = std::stacktrace::current(),
	              const std::source_location& location = std::source_location::current());

	void Success(const std::string&          aString, bool withNotice = false,
	             const std::source_location& location = std::source_location::current());

	void Critical(const std::exception&       anException, unsigned aLevel = 0, bool withNotice = false,
	              const std::source_location& location = std::source_location::current());

	void Critical(const std::string&          anExceptionText, unsigned aLevel = 0, bool withNotice = false,
	              const std::source_location& location = std::source_location::current());

	static void NewLine();
	void        Clear();

	void* GetHandle() const
	{
		return myHandle;
	}

	template <typename T>
	void Log(const T& aString,bool withNotice = false,
		const std::source_location& location = std::source_location::current()) {
		Log(std::to_string(aString),withNotice,location);
	}

	template <typename... Args>
	void LogC(Args&&... args) {
		std::ostringstream stream;
		(stream << ... << std::forward<Args>(args));
		Log(std::string(stream.str()));
	}

	template <typename... Args>
	void WarnC(Args&&... args) {
		std::ostringstream stream;
		(stream << ... << std::forward<Args>(args));
		Warn(std::string(stream.str()));
	}

	template <typename... Args>
	void ErrC(Args&& ... args) {
		std::ostringstream stream;
		(stream << ... << std::forward<Args>(args));
		Err(stream.str());
	}

	std::mutex & mutexLock() {
		return readyToWrite;
	}
private:
	void*                            myHandle = nullptr;
	bool                             shouldPrintToOutput = false;
	bool                             isInitialized = false;
	std::string                      myNamespace;
	std::mutex                       readyToWrite;
	[[nodiscard]] static std::string Timestamp();
};

ENABLE_ENUM_BITWISE_OPERATORS(LoggerService::LogType)

