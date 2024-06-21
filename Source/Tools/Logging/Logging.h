#pragma once
#define NOMINMAX
#include <mutex>
#include <source_location> 
#include <string>
#include <vector>

#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include <Tools/Utilities/TemplateHelpers.h>
class Logger
{
public:
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

		void Add(LogMsg msg)
		{
			switch (msg.messageType)
			{
				using enum Logger::LogType;
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
				std::unreachable();
				break;
			}
			LoggedMessages.emplace_back(msg);
		}

		unsigned messagesCount{};
		unsigned warnCount{};
		unsigned errCount{};
		unsigned criticalCount{};
		unsigned successCount{};
	};
public:
	static inline logBuffer m_Buffer;
	static bool Create();
	static void SetConsoleHandle(void* aHandle);
	static void SetPrintToVSOutput(bool bNewValue);
	static void Log(const char* aString);
	static Vector3f GetColor(LogType type);

	template<typename T>
	static void  Log(const T& aString)
	{
		Log(std::to_string(aString));
	}

	static void Log(const std::string& aString);
	static void Warn(const std::string& aString);
	static void Err(const std::string& aString,const std::source_location& location =
		std::source_location::current());
	static void Succ(const std::string& aString);
	static void Critical(const std::exception& anException,unsigned aLevel = 0,const std::source_location& location =
		std::source_location::current());
	static void Critical(const std::string& anExceptionText,unsigned aLevel = 0,const std::source_location& location =
		std::source_location::current());
	static void NewLine();
	void* GetHandle() const { return myHandle; }
private:
	static inline void* myHandle = 0;
	static inline bool shouldPrintToOutput = false;
	static inline bool isInitialized = false;
	static inline std::string myNamespace;
	static inline std::mutex readyToWrite;
	static [[nodiscard]] std::string Timestamp();
	Logger() = default;

};


ENABLE_ENUM_BITWISE_OPERATORS(Logger::LogType)