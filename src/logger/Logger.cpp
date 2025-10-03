#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include "../common/constants.hpp"
#include <sstream>

Logger::Logger() : _consoleOutput(true), _fileOutput(false)
{
}

Logger::Logger(const std::string &logFile) : _logFileName(logFile), _consoleOutput(true), _fileOutput(true)
{
    _logFile.open(_logFileName, std::ios::app);
}

Logger::Logger(const std::string &logFile, bool consoleOutput, bool fileOutput)
    : _logFileName(logFile), _consoleOutput(consoleOutput), _fileOutput(fileOutput)
{
    if (_fileOutput)
    {
        _logFile.open(_logFileName, std::ios::app);
    }
}

Logger::~Logger()
{
    if (_logFile.is_open())
        _logFile.close();
}

void Logger::setLogFile(const std::string &filename)
{
    if (_logFile.is_open())
        _logFile.close();
    _logFileName = filename;
    if (_fileOutput)
        _logFile.open(_logFileName, std::ios::app);
}

void Logger::enableConsoleOutput(bool enable)
{
    _consoleOutput = enable;
}

void Logger::enableFileOutput(bool enable)
{
    _fileOutput = enable;
    if (_fileOutput && !_logFile.is_open() && !_logFileName.empty())
        _logFile.open(_logFileName, std::ios::app);
    else if (!_fileOutput && _logFile.is_open())
        _logFile.close();
}

void Logger::log(LogLevel level, Component component, const std::string &message)
{
    std::string levelStr;
    switch (level)
    {
    case DEBUG:
        levelStr = ANSI_PURPLE "DEBUG" ANSI_RESET;
        break;
    case INFO:
        levelStr = ANSI_BLUE "INFO" ANSI_RESET;
        break;
    case SUCCESS:
        levelStr = ANSI_GREEN "SUCCESS" ANSI_RESET;
        break;
    case WARNING:
        levelStr = ANSI_YELLOW "WARNING" ANSI_RESET;
        break;
    case ERROR:
        levelStr = ANSI_ORANGE "ERROR" ANSI_RESET;
        break;
    case FATAL:
        levelStr = ANSI_RED "FATAL" ANSI_RESET;
        break;
    }

    std::string componentStr;
    switch (component)
    {
    case SERVER:
        componentStr = "SERVER";
        break;
    case CLIENT:
        componentStr = "CLIENT";
        break;
    case CHANNEL:
        componentStr = "CHANNEL";
        break;
    case AUTH:
        componentStr = "AUTH";
        break;
    case COMMAND:
        componentStr = "COMMAND";
        break;
    case NETWORK:
        componentStr = "NETWORK";
        break;
    case PARSER:
        componentStr = "PARSER";
        break;
    }

    std::string formattedMessage;
    if (levelStr == "SUCCESS")
        formattedMessage = "[" + getCurrentTimestamp() + "] [" + levelStr + "] [" + componentStr + "] " + message;
    else
        formattedMessage = "[" + getCurrentTimestamp() + "] [" + levelStr + "]\t[" + componentStr + "] " + message;
    writeLog(formattedMessage);
}

void Logger::debug(Component component, const std::string &message)
{
    log(DEBUG, component, message);
}

void Logger::info(Component component, const std::string &message)
{
    log(INFO, component, message);
}

void Logger::success(Component component, const std::string &message)
{
    log(SUCCESS, component, message);
}

void Logger::warning(Component component, const std::string &message)
{
    log(WARNING, component, message);
}

void Logger::error(Component component, const std::string &message)
{
    log(ERROR, component, message);
}

void Logger::fatal(Component component, const std::string &message)
{
    log(FATAL, component, message);
}

std::string Logger::getCurrentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void Logger::writeLog(const std::string &formattedMessage)
{
    if (_consoleOutput)
        std::cout << formattedMessage << std::endl;

    if (_fileOutput && _logFile.is_open())
    {
        _logFile << formattedMessage << std::endl;
        _logFile.flush();
    }
}