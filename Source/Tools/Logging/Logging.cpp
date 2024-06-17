#include "Logging.h"

#include <iomanip>
#include <iostream>
#include <ostream> 
#include <sstream>
#include "Editor/Editor/Core/Editor.h"
#include "Tools/Utilities/DataStructures/Queue.hpp"

std::string Logger::Timestamp()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

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

bool Logger::Create()
{
	isInitialized = true;
	SetConsoleHandle(GetConsoleWindow());
	Succ("Logging started");
	return true;
}

void Logger::SetConsoleHandle(HANDLE aHandle)
{
	myHandle = aHandle;
}

void Logger::SetPrintToVSOutput(bool bNewValue)
{
	shouldPrintToOutput = bNewValue;
}

void Logger::Log(const char* aString)
{
	Log(std::string(aString));
}

Vector3f Logger::GetColor(LogType type)
{
	switch (type)
	{
	using enum Logger::LogType;
	case message:
		return Vector3f(1.0f,1.0f,1.0f);
		break;
	case warning:
		return Vector3f(1.0f,1.0f,0.0f);
		break;
	case error:
		return Vector3f(1.0f,0.0f,0.0f);
		break;
	case critical:
		return Vector3f(1.0f,0.0f,0.0f);
		break; 
	case success:
		return Vector3f(0.0f,1.0f,0.0f);
		break;
	default:
		std::unreachable();
		break;
	} 
}

void Logger::Log(const std::string& aString)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.messageType = LogType::message;
			msg.message = "[" + Timestamp() + "]" + myNamespace + " [   LOG   ] " + aString;  
			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
			std::cout << "[   LOG   ]";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Warn(const std::string& aString)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.message = "[" + Timestamp() + "]" + myNamespace + " [ WARNING ] " + aString;
			msg.messageType = LogType::warning;

			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_RED | BACKGROUND_GREEN);
			std::cout << "[ WARNING ]";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Err(const std::string& aString,const std::source_location& location)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.message = "[" + Timestamp() + "]" + myNamespace + " [  ERROR  ] " + aString;
			msg.messageType = LogType::error;

			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_RED);
			std::cout << "[  ERROR  ]";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout << " " << aString << "\n";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		std::wcout << "file: "
			<< location.file_name() << '('
			<< location.line() << ':'
			<< location.column() << ") `"
			<< location.function_name() << std::endl;
	}
}

void Logger::Succ(const std::string& aString)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.message = "[" + Timestamp() + "]" + "[ SUCCESS ] " + aString;
			msg.messageType = LogType::success; 
			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_GREEN);
			std::cout << "[ SUCCESS ]";
			SetConsoleTextAttribute(myHandle,FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			std::cout << " " << aString << std::endl;
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
	}
}

void Logger::Critical(const std::exception& anException,unsigned aLevel,const std::source_location& location)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.message = "[" + Timestamp() + "]" + std::string(aLevel,' ') + "[  FATAL  ] " + anException.what();
			msg.messageType = LogType::critical; 
			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_RED);
			std::wcout << "[  FATAL  ]";
			std::wcout << std::string(aLevel,' ').c_str();
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::wcout << " " << anException.what() << "\n";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		std::wcout << "file: "
			<< location.file_name() << '('
			<< location.line() << ':'
			<< location.column() << ") `"
			<< location.function_name() << std::endl;

		try
		{
			std::rethrow_if_nested(anException);
		}
		catch (const std::exception& nestedException)
		{
			Critical(nestedException,aLevel + 1);
		}
		catch (...) {} // Catch all other cases.
	}
}

void Logger::Critical(const std::string& anExceptionText,unsigned aLevel,const std::source_location& location)
{
	if (isInitialized)
	{
		std::scoped_lock lock(readyToWrite);
		if (shouldPrintToOutput)
		{
			LogMsg msg;
			msg.message = "[" + Timestamp() + "]" + std::string(aLevel,' ') + "[  FATAL  ] " + anExceptionText;
			msg.messageType = LogType::critical;
			OutputDebugStringA(msg.message.c_str());
			m_Buffer.Add(msg);
		}
		else
		{
			std::cout << "[" << Timestamp() << "] ";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			std::cout << myNamespace << " ";
			SetConsoleTextAttribute(myHandle,BACKGROUND_RED);
			std::wcout << "[  FATAL  ]";
			std::wcout << std::string(aLevel,' ').c_str();
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::wcout << " " << anExceptionText.c_str() << "\n";
			SetConsoleTextAttribute(myHandle,FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}

		std::wcout << "file: "
			<< location.file_name() << '('
			<< location.line() << ':'
			<< location.column() << ") `"
			<< location.function_name() << std::endl;

		throw anExceptionText;
	}
}

void Logger::NewLine()
{
	std::cout << std::endl;
}
