#pragma once
#define NOMINMAX
#include <string>

#include "Windows.h"
#include <source_location> 
class Logger
{
	HANDLE myHandle = 0;
	bool shouldPrintToOutput = false;
	bool isInitialized = false;
	std::string myNamespace;

	Logger(const std::string& aNamespace);
	[[nodiscard]] std::string Timestamp() const;

public:

	Logger() = default;

	static Logger Create(const std::string& aNamespace);

	void SetConsoleHandle(HANDLE aHandle);

	// Tells the logger to output to the Visual Studio output window instead
	// of a console.
	void SetPrintToVSOutput(bool bNewValue);

	// Log a message.
	void Log(const std::string& aString) const;

	// Log a warning message.
	void Warn(const std::string& aString) const;

	// Log an error.
	void Err(const std::string& aString,const std::source_location location =
		std::source_location::current()) const;

	// Log a success message.
	void Succ(const std::string& aString) const;

	// Log an exception. Will step through nested exceptions if there are any.
	void LogException(const std::exception& anException,unsigned aLevel = 0,const std::source_location location =
		std::source_location::current()) const;

	// Just force the log to go to next line.
	void NewLine() const;

	FORCEINLINE HANDLE GetHandle() const { return myHandle; }
};