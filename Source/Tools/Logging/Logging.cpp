#include "Logging.h"

#include "Windows.h"
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

std::string LoggerService::Timestamp()
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    std::stringstream result;
    result << std::setfill('0') << std::setw(2) << st.wHour;
    result << ":";
    result << std::setfill('0') << std::setw(2) << st.wMinute;
    result << ":";
    result << std::setfill('0') << std::setw(2) << st.wSecond;

    return result.str();
}

bool LoggerService::Create()
{
    isInitialized = true;
    SetConsoleHandle(GetConsoleWindow());
    Succ("Logging started");
    return true;
}

void LoggerService::SetConsoleHandle(void *aHandle)
{
    myHandle = aHandle;
}

void LoggerService::SetPrintToVSOutput(bool bNewValue)
{
    shouldPrintToOutput = bNewValue;
}

void LoggerService::Log(const char *aString, const std::source_location &location)
{
    Log(std::string(aString), location);
}

Color LoggerService::GetColor(LogType type)
{
    switch (type)
    {
        using enum LoggerService::LogType;
    case message:
        return Color(1.0f, 1.0f, 1.0f);
        break;
    case warning:
        return Color(1.0f, 1.0f, 0.0f);
        break;
    case error:
        return Color(1.0f, 0.0f, 0.0f);
        break;
    case critical:
        return Color(1.0f, 0.0f, 0.0f);
        break;
    case success:
        return Color(0.0f, 1.0f, 0.0f);
        break;
    default:
        std::unreachable();
        break;
    }
}

void LoggerService::Log(const std::string &aString, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;
            msg.messageType = LogType::message;
            std::filesystem::path sourceFile = location.file_name();
            msg.message = std::format("[LOG] [{}] {} {} {} -> {}", Timestamp(), sourceFile.filename().string(),
                                      location.function_name(), location.line(), aString);
            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
        }
        else
        {
            std::cout << "[" << Timestamp() << "] ";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            std::cout << myNamespace << " ";
            SetConsoleTextAttribute(myHandle, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
            std::cout << "[   LOG   ]";
            SetConsoleTextAttribute(myHandle,
                                    FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            std::cout << " " << aString << std::endl;
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
}

void LoggerService::Warn(const std::string &aString, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;
            std::filesystem::path sourceFile = location.file_name();
            msg.message = std::format("[WARNING] [{}] {} {} {} -> {}", Timestamp(), sourceFile.filename().string(),
                                      location.function_name(), location.line(), aString);
            msg.messageType = LogType::warning;

            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
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

void LoggerService::Err(const std::string &aString, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;
            std::filesystem::path sourceFile = location.file_name();
            msg.message = std::format("[ERROR] [{}] {} {} {} -> {}", Timestamp(), sourceFile.filename().string(),
                                      location.function_name(), location.line(), aString);
            msg.messageType = LogType::error;

            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
            // TODO: print location
        }
        else
        {
            std::cout << "[" << Timestamp() << "] ";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            std::cout << myNamespace << " ";
            SetConsoleTextAttribute(myHandle, BACKGROUND_RED);
            std::cout << "[  ERROR  ]";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
            std::cout << " " << aString << "\n";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        std::wcout << "file: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") `"
                   << location.function_name() << std::endl;
    }
}
#pragma optimize("", off)

void LoggerService::Succ(const std::string &aString, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;
            std::filesystem::path sourceFile = location.file_name();
            msg.message = std::format("[SUCCESS] [{}] {} {} {} -> {}", Timestamp(), sourceFile.filename().string(),
                                      location.function_name(), location.line(), aString);
            msg.messageType = LogType::success;
            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
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

void LoggerService::Critical(const std::exception &anException, unsigned aLevel, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;

            std::filesystem::path sourceFile = location.file_name();
            msg.message =
                std::format("[FATAL] [{}] {} {} {} \nSeverity {}\n{}", Timestamp(), sourceFile.filename().string(),
                            location.function_name(), location.line(), aLevel, anException.what());

            msg.messageType = LogType::critical;
            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
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
            std::wcout << " " << anException.what() << "\n";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        std::wcout << "file: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") `"
                   << location.function_name() << std::endl;

        try
        {
            std::rethrow_if_nested(anException);
        }
        catch (const std::exception &nestedException)
        {
            Critical(nestedException, aLevel + 1);
        }
        catch (...)
        {
        } // Catch all other cases.
    }
}

// Critical error will proberly throw somewhere close, if caught and handeled like by a script holder it can still print
// what went bad
void LoggerService::Critical(const std::string &anExceptionText, unsigned aLevel, const std::source_location &location)
{
    if (isInitialized)
    {
        std::scoped_lock lock(readyToWrite);
        if (shouldPrintToOutput)
        {
            LogMsg msg;
            std::filesystem::path sourceFile = location.file_name();
            msg.message =
                std::format("[FATAL] [{}] {} {} {} \nSeverity {}\n{}", Timestamp(), sourceFile.filename().string(),
                            location.function_name(), location.line(), aLevel, anExceptionText);
            msg.messageType = LogType::critical;
            OutputDebugStringA(msg.message.c_str());
            m_Buffer.Add(msg);
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
            std::wcout << " " << anExceptionText.c_str() << "\n";
            SetConsoleTextAttribute(myHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        std::wcout << "file: " << location.file_name() << '(' << location.line() << ':' << location.column() << ") `"
                   << location.function_name() << std::endl;

        throw anExceptionText;
    }
}

void LoggerService::NewLine()
{
    std::cout << std::endl;
}
