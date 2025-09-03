#pragma once

#include <string>
#include <fstream>
#include <sstream>

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};
enum Component
{
    SERVER,
    CLIENT,
    CHANNEL,
    AUTH,
    COMMAND,
    NETWORK,
    PARSER
};

class Logger
{
public:
    Logger();
    explicit Logger(const std::string &logFile);
    Logger(const std::string &logFile, bool consoleOutput, bool fileOutput);
    ~Logger();

    void setLogFile(const std::string &filename);
    void enableConsoleOutput(bool enable);
    void enableFileOutput(bool enable);

    void log(LogLevel level, Component component, const std::string &message);
    void debug(Component component, const std::string &message);
    void info(Component component, const std::string &message);
    void warning(Component component, const std::string &message);
    void error(Component component, const std::string &message);
    void fatal(Component component, const std::string &message);

    static std::string getCurrentTimestamp();

private:
    std::ofstream _logFile;
    std::string _logFileName;
    bool _consoleOutput;
    bool _fileOutput;

    void writeLog(const std::string &formattedMessage);
};