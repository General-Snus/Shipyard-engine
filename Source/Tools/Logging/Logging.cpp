#include "Logging.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

Logger::Logger(const std::string& aNamespace)
{
	myNamespace = "[ " + aNamespace + " ]";
	isInitialized = true;
}

std::string Logger::Timestamp() const
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	std::stringstream result;
	result << std::setfill('0') << std::setw(2) << st.wYear;
	result << "/";
	result << std::setfill('0') << std::setw(2) << st.wMonth;
	result << "/";
	result << std::setfill('0') << std::setw(2) << st.wDay;
	result << " - ";
	result << std::setfill('0') << std::setw(2) << st.wHour;
	result << ":";
	result << std::setfill('0') << std::setw(2) << st.wMinute;
	result << ":";
	result << std::setfill('0') << std::setw(2) << st.wSecond;

	return result.str();
}

Logger Logger::Create(const std::string& aNamespace)
{
	Logger aLogger(aNamespace);
	aLogger.SetConsoleHandle(GetConsoleWindow());
	aLogger.Succ("Logging started for " + aNamespace);
	return aLogger;
}

void Logger::SetConsoleHandle(HANDLE aHandle)
{
	myHandle = aHandle;
}

void Logger::SetPrintToVSOutput(bool bNewValue)
{
	shouldPrintToOutput = bNewValue;
}

void Logger::Log(const std::string& aString) const
{
	if(isInitialized)
	{

		if (shouldPrintToOutput)
		{
			const std::string message = "[" + Timestamp() + "]" + myNamespace + " [   LOG   ] " + aString;
			OutputDebugStringA(message.c_str());
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
			std::cout << "[   LOG   ]";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Warn(const std::string& aString) const
{
	if (isInitialized)
	{
		if (shouldPrintToOutput)
		{
			const std::string message = "[" + Timestamp() + "]" + myNamespace + " [ WARNING ] " + aString;
			OutputDebugStringA(message.c_str());
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle, BACKGROUND_RED | BACKGROUND_GREEN);
			std::cout << "[ WARNING ]";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Err(const std::string& aString) const
{
	if (isInitialized)
	{
		if (shouldPrintToOutput)
		{
			const std::string message = "[" + Timestamp() + "]" + myNamespace + " [  ERROR  ] " + aString;
			OutputDebugStringA(message.c_str());
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle, BACKGROUND_RED);
			std::cout << "[  ERROR  ]";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Succ(const std::string& aString) const
{
	if (isInitialized)
	{
		if (shouldPrintToOutput)
		{
			const std::string message = "[" + Timestamp() + "]" + "[ SUCCESS ] " + aString;
			OutputDebugStringA(message.c_str());
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle, BACKGROUND_GREEN);
			std::cout << "[ SUCCESS ]";
			SetConsoleTextAttribute(myHandle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::LogException(const std::exception& anException, unsigned aLevel) const
{
	if (isInitialized)
	{
		if (shouldPrintToOutput)
		{
			const std::string message = "[" + Timestamp() + "]" + std::string(aLevel, ' ') + "[  FATAL  ] " + anException.what();
			OutputDebugStringA(message.c_str());
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle, BACKGROUND_RED);
			std::wcout << "[  FATAL  ]";
			std::wcout << std::string(aLevel, ' ').c_str();
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::wcout << " " << anException.what() << std::endl;
			SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		try
		{
			std::rethrow_if_nested(anException);
		}
		catch (const std::exception& nestedException)
		{
			LogException(nestedException, aLevel + 1);
		}
		catch (...) {} // Catch all other cases.
	}
}

void Logger::NewLine() const
{
	std::cout << std::endl;
}
