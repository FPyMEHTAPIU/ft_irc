#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>

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
    case LogLevel::DEBUG:
        levelStr = "DEBUG";
        break;
    case LogLevel::INFO:
        levelStr = "INFO";
        break;
    case LogLevel::WARNING:
        levelStr = "WARNING";
        break;
    case LogLevel::ERROR:
        levelStr = "ERROR";
        break;
    case LogLevel::FATAL:
        levelStr = "FATAL";
        break;
    }

    std::string componentStr;
    switch (component)
    {
    case Component::SERVER:
        componentStr = "SERVER";
        break;
    case Component::CLIENT:
        componentStr = "CLIENT";
        break;
    case Component::CHANNEL:
        componentStr = "CHANNEL";
        break;
    case Component::AUTH:
        componentStr = "AUTH";
        break;
    case Component::COMMAND:
        componentStr = "COMMAND";
        break;
    case Component::NETWORK:
        componentStr = "NETWORK";
        break;
    case Component::PARSER:
        componentStr = "PARSER";
        break;
    }

    std::string formattedMessage = "[" + getCurrentTimestamp() + "] [" + levelStr + "] [" + componentStr + "] " + message;
    writeLog(formattedMessage);
}

void Logger::debug(Component component, const std::string &message)
{
    log(LogLevel::DEBUG, component, message);
}

void Logger::info(Component component, const std::string &message)
{
    log(LogLevel::INFO, component, message);
}

void Logger::warning(Component component, const std::string &message)
{
    log(LogLevel::WARNING, component, message);
}

void Logger::error(Component component, const std::string &message)
{
    log(LogLevel::ERROR, component, message);
}

void Logger::fatal(Component component, const std::string &message)
{
    log(LogLevel::FATAL, component, message);
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