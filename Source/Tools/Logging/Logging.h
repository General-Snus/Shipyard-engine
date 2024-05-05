#pragma once
#define NOMINMAX
#include <mutex>
#include <source_location> 
#include <string>
#include <vector>

#include "Tools/Utilities/LinearAlgebra/Vector3.hpp"
#include "Windows.h"

class Logger
{
	struct LogMsg
	{
		Vector3f myColor;
		std::string message;
	};
	static inline HANDLE myHandle = 0;
	static inline bool shouldPrintToOutput = false;
	static inline bool isInitialized = false;
	static inline std::string myNamespace;
	static inline std::mutex readyToWrite;
	static [[nodiscard]] std::string Timestamp();
	Logger() = default;

public:
	static inline std::vector<LogMsg> m_LogMsgs;
	static bool Create();
	static void SetConsoleHandle(HANDLE aHandle);

	// Tells the logger to output to the Visual Studio output window instead
	// of a console.
	static void SetPrintToVSOutput(bool bNewValue);

	// Log a message.
	static void Log(const std::string& aString);

	// Log a warning message.
	static void Warn(const std::string& aString);

	// Log an error.
	static void Err(const std::string& aString,const std::source_location& location =
		std::source_location::current());

	// Log a success message.
	static void Succ(const std::string& aString);

	// Log an exception. Will step through nested exceptions if there are any.
	static void LogException(const std::exception& anException,unsigned aLevel = 0,const std::source_location& location =
		std::source_location::current());

	// Just force the log to go to next line.
	static void NewLine();

	FORCEINLINE HANDLE GetHandle() const { return myHandle; }
};
